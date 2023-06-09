{-# LANGUAGE OverloadedStrings, RecordWildCards, DeriveFunctor, DeriveFoldable, DeriveTraversable #-}

module SylvanSolver where

import Control.Applicative
import qualified  Data.Text.IO as T
import Data.Map.Strict (Map)
import qualified  Data.Map.Strict as Map
import Control.Monad.ST.Strict
import Control.Monad
import Data.List
import Data.Tuple.All
import Data.Bits
import Control.Monad.ST.Unsafe
import System.Directory
import System.IO
import System.CPUTime
import Data.Foldable (Foldable)
import Data.Traversable (Traversable)
import qualified Data.Traversable as T
import Control.Monad.Trans.Except
import Control.Error.Util
import Control.Monad.Trans
import Control.Arrow
import Data.Monoid

import Options.Applicative as O
import Safe
import Data.Attoparsec.Text as P

import qualified Sylvan.Sylvan as S
import Sylvan.Sylvan (BDD, BDDVar, BDDMap)

import AAG

--BDD operations
data Ops s v m a = Ops {
    bAnd          :: a -> a -> ST s a,
    bOr           :: a -> a -> ST s a,
    bImp          :: a -> a -> ST s a,
    lEq           :: a -> a -> ST s Bool,
    neg           :: a -> a,
    constructMap  :: [(Int, a)] -> ST s m,
    vectorCompose :: a -> m -> ST s a,
    computeVarSet :: [Int] -> ST s v,
    computeCube   :: v -> [Bool] -> ST s a,
    ithVar        :: Int -> ST s a,
    bforall       :: v -> a -> ST s a,
    bexists       :: v -> a -> ST s a,
    deref         :: a -> ST s (),
    ref           :: a -> ST s (),
    btrue         :: a,
    bfalse        :: a,
    andAbstract   :: v -> a -> a -> ST s a
}

constructOps :: Ops s BDD BDDMap BDD
constructOps = Ops {..}
    where
    bAnd x y          = do
        res <- S.band x y
        ref res
        S.testReduceHeap
        return res
    bOr x y           = do
        res <- S.bor x y
        ref res
        return res
    bImp x y          = do
        res <- S.bimp x y
        ref res
        return res
    lEq x y           = do
        res <- x `bImp` y
        deref res
        return $ res == btrue
    neg               = S.neg
    constructMap ps   = do
        S.gcDisable
        res <- foldM func S.mapEmpty ps
        S.gcEnable
        return res
        where
        func m (v, x) = do
            res <- S.mapAdd m (fromIntegral v) x
            S.refMap res
            S.derefMap m
            return res
    vectorCompose x m = do
        res <- S.compose x m
        ref res
        return res
    computeVarSet is  = do
        res <- S.setFromArray (map fromIntegral is)
        ref res
        return res
    computeCube v p   = do
        res <- S.cube v (map func p)
        ref res
        return res
        where
        func False = S.Negative
        func True  = S.Positive
    ithVar i          = do
        res <- S.ithVar $ fromIntegral i
        ref res
        return res
    bforall v x       = do
        res <- S.forall x v
        ref res
        return res
    bexists v x       = do
        res <- S.exists x v
        ref res
        return res
    deref             = S.deref 
    ref               = void . S.ref
    btrue             = S.sylvanTrue
    bfalse            = S.sylvanFalse
    andAbstract v x y = do
        xy <- S.andExists x y v
        ref xy
        return xy

bddSynopsis :: (Show a, Eq a) => Ops s v m a -> a -> ST s ()
bddSynopsis Ops{..} x 
    | x == btrue  = unsafeIOToST $ putStrLn "True"
    | x == bfalse = unsafeIOToST $ putStrLn "False"
    | otherwise   = unsafeIOToST $ print x

--Compiling the AIG
makeAndMap :: [(Int, Int, Int)] -> Map Int (Int, Int)
makeAndMap = Map.fromList . map func
    where func (out, x, y) = (out, (x, y))

doAndGates :: Ops s v m a -> Map Int (Int, Int) -> Map Int a -> Int -> ST s (Map Int a, a)
doAndGates ops@Ops{..} andGateInputs accum andGate = 
    case Map.lookup andGate accum of
        Just x  -> return (accum, x)
        Nothing -> do
            let varIdx   =  clearBit andGate 0
                (x, y)   =  fromJustNote "doAndGates" $ Map.lookup varIdx andGateInputs
            (accum', x)  <- doAndGates ops andGateInputs accum x
            (accum'', y) <- doAndGates ops andGateInputs accum' y
            res          <- bAnd x y
            let finalMap =  Map.insert varIdx res (Map.insert (varIdx + 1) (neg res) accum'')
            return (finalMap, fromJustNote "doAndGates2" $ Map.lookup andGate finalMap) 

mapAccumLM :: Monad m => (acc -> x -> m (acc, y)) -> acc -> [x] -> m (acc, [y])
mapAccumLM _ s []     = return (s, [])
mapAccumLM f s (x:xs) = do
    (s1, x')  <- f s x
    (s2, xs') <- mapAccumLM f s1 xs
    return    (s2, x' : xs')

--Synthesis
data SynthState v m a = SynthState {
    cInputCube :: v,
    uInputCube :: v,
    safeRegion :: a,
    trel       :: m,
    initState  :: a
} deriving (Functor, Foldable, Traversable)

substitutionArray :: Ops s v m a -> Map Int Int -> Map Int a -> ST s m
substitutionArray Ops{..} latches andGates = constructMap pairs
    where
    ls = Map.toList latches 
    pairs = map ((\x -> (x `quot` 2) - 1) *** fromJustNote "substitutionArray" . flip Map.lookup andGates) ls

compile :: Ops s v m a -> [Int] -> [Int] -> [(Int, Int)] -> [(Int, Int, Int)] -> Int -> ST s (SynthState v m a)
compile ops@Ops{..} controllableInputs uncontrollableInputs latches ands safeIndex = do
    let andGates = map sel1 ands
        andMap   = makeAndMap ands

    --create an entry for each controllable input 
    let nextIdx       = length controllableInputs
        cInputIndices = [0 .. nextIdx - 1]

    cInputVars <- mapM ithVar cInputIndices
    cInputCube <- computeVarSet cInputIndices

    --create an entry for each uncontrollable input
    let nextIdx2      = nextIdx + length uncontrollableInputs
        uInputIndices = [nextIdx .. nextIdx2 - 1]
    uInputVars <- mapM ithVar uInputIndices
    uInputCube <- computeVarSet uInputIndices

    --create an entry for each latch 
    let nextIdx3     = nextIdx2 + length latches
        latchIndices = [nextIdx2 .. nextIdx3 - 1]
    latchVars  <- mapM ithVar latchIndices
    latchCube  <- computeVarSet latchIndices

    ref btrue
    ref bfalse

    --create the symbol table
    let func idx var = [(idx, var), (idx + 1, neg var)]
        tf   = Map.fromList $ [(0, bfalse), (1, btrue)]
        mpCI = Map.fromList $ concat $ zipWith func controllableInputs cInputVars
        mpUI = Map.fromList $ concat $ zipWith func uncontrollableInputs uInputVars
        mpL  = Map.fromList $ concat $ zipWith func (map fst latches) latchVars
        im   = Map.unions [tf, mpCI, mpUI, mpL]

    --compile the and gates
    stab     <- fst <$> mapAccumLM (doAndGates ops andMap) im andGates

    -- S.testReduceHeap
    
    --get the safety condition
    let sr   = fromJustNote "compile" $ Map.lookup safeIndex stab
    
    
    --construct the initial state
    initState <- computeCube latchCube (replicate (length latchVars) False)

    --construct the transition relation
    let latchMap = Map.fromList latches
    trel <- substitutionArray ops latchMap stab

    ref sr
    let func k v = when (even k) (deref v)
    Map.traverseWithKey func stab

    return $ SynthState cInputCube uInputCube (neg sr) trel initState

safeCpre :: (Show a, Eq a) => Bool -> Ops s v m a -> SynthState v m a -> a -> ST s a
safeCpre quiet ops@Ops{..} SynthState{..} s = do
    unless quiet $ unsafeIOToST $ print "*"
    scu' <- vectorCompose s trel

    scu <- andAbstract cInputCube safeRegion scu'
    deref scu'

    s   <- bforall uInputCube scu
    deref scu
    return s

fixedPoint :: Eq a => Ops s v m a -> a -> a -> (a -> ST s a) -> ST s Bool
fixedPoint ops@Ops{..} init start func = do
    res <- func start
    deref start
    win <- init `lEq` res
    case win of
        False -> deref res >> return False
        True  -> 
            case (res == start) of
                True  -> deref res >> return True
                False -> fixedPoint ops init res func 

solveSafety :: (Eq a, Show a) => Bool -> Ops s v m a -> SynthState v m a -> a -> a -> ST s Bool
solveSafety quiet ops@Ops{..} ss init safeRegion = do
    ref btrue
    fixedPoint ops init btrue (safeCpre quiet ops ss) 

categorizeInputs :: [Symbol] -> [Int] -> ([Int], [Int])
categorizeInputs symbols inputs = (cont, inputs \\ cont)
    where
    cont                     = map (inputs !!) theSet
    theSet                   = map idx $ filter (isControllable . typ) symbols
    isControllable (Is Cont) = True
    isControllable _         = False

doIt :: Options -> IO (Either String Bool)
doIt (Options {..}) = runExceptT $ do
    contents    <- lift $ T.readFile filename
    aag@AAG{..} <- hoistEither $ parseOnly aag contents
    lift $ do
        let (cInputs, uInputs) = categorizeInputs symbols inputs
        stToIO $ do
            S.laceStart 8 0

            S.setLimits (1 `shiftL` 21) 1 0

            S.initPackage
            S.initMtbdd 
            S.initReorder

            S.setReorderNodesThreshold 1
            S.setReorderTimeLimitSec 60
            S.setReorderMaxGrowth 1.2

            S.gcEnable

            let ops = constructOps 
            ss@SynthState{..} <- compile ops cInputs uInputs latches andGates (head outputs)
            res <- solveSafety quiet ops ss initState safeRegion
            T.mapM (deref ops) ss
            S.sylvanQuit
            return res

run :: Options -> IO ()
run g = do
    res <- doIt g 
    case res of
        Left err    -> putStrLn $ "Error: " ++ err
        Right True  -> putStrLn "REALIZABLE"
        Right False -> putStrLn "UNREALIZABLE"

data Options = Options {
    quiet    :: Bool,
    threads  :: Int,
    filename :: String
}
