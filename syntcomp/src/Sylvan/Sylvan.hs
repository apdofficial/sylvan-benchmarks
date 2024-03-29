module Sylvan.Sylvan where

import Foreign.C.Types
import Foreign.Ptr
import Foreign.Marshal.Array
import Data.Word
import Control.Monad
import Data.Bits
import Control.Monad.Primitive

--Lace
foreign import ccall safe "lace_start"
    c_laceStart :: CInt -> CInt -> IO ()

laceStart :: PrimMonad m => Int -> Int -> m ()
laceStart workers deque = unsafePrimToPrim $ c_laceStart (fromIntegral workers) (fromIntegral deque)

--Sylvan
type    CBDD    = CLLong
newtype BDD     = BDD CBDD deriving (Eq, Show)
type    CBDDVar = CUInt
type    BDDVar  = Word32
type    CBDDMap = CLLong
newtype BDDMap  = BDDMap CBDDMap deriving (Show)

c_sylvanComplement :: CLLong 
c_sylvanComplement =  0x8000000000000000
c_sylvanFalse      :: CLLong 
c_sylvanFalse      =  0x0000000000000000
c_sylvanTrue       :: CLLong 
c_sylvanTrue       =  c_sylvanFalse .|. c_sylvanComplement

sylvanFalse = BDD c_sylvanFalse
sylvanTrue  = BDD c_sylvanTrue

--Init
foreign import ccall safe "sylvan_init_package"
    c_sylvanInitPackage :: IO ()

initPackage :: PrimMonad m => m ()
initPackage = unsafePrimToPrim c_sylvanInitPackage 

foreign import ccall safe "sylvan_init_mtbdd"
    c_sylvanInitMtbdd :: IO ()

initMtbdd :: PrimMonad m => m ()
initMtbdd = unsafePrimToPrim c_sylvanInitMtbdd 

foreign import ccall safe "sylvan_init_reorder"
    c_sylvanInitReorder :: IO ()

initReorder :: PrimMonad m => m ()
initReorder = unsafePrimToPrim c_sylvanInitReorder


--Setters
foreign import ccall safe "sylvan_set_limits"
    c_sylvanSetLimits :: CSize -> CInt -> CInt -> IO ()

setLimits :: PrimMonad m => Int -> Int -> Int -> m ()
setLimits memoryCap tableRatio initialRatio = unsafePrimToPrim $ c_sylvanSetLimits (fromIntegral memoryCap) (fromIntegral tableRatio) (fromIntegral initialRatio)

foreign import ccall safe "sylvan_set_sizes"
    c_sylvanSetSizes :: CSize -> CSize -> CSize -> CSize -> IO ()

setSizes :: PrimMonad m => Int -> Int -> Int -> Int -> m ()
setSizes minTableSize maxTableSize minCacheSize maxCacheSize = unsafePrimToPrim $ c_sylvanSetSizes (fromIntegral minTableSize) (fromIntegral maxTableSize) (fromIntegral minCacheSize) (fromIntegral maxCacheSize)

--Setters
foreign import ccall safe "sylvan_quit"
    c_sylvanQuit :: IO ()

sylvanQuit :: PrimMonad m => m ()
sylvanQuit = unsafePrimToPrim c_sylvanQuit

foreign import ccall safe "mtbdd_ithvar"
    c_ithVar :: CBDDVar -> IO CBDD

ithVar :: PrimMonad m => BDDVar -> m BDD
ithVar var = liftM BDD $ unsafePrimToPrim $ c_ithVar (fromIntegral var)

nithVar :: PrimMonad m => BDDVar -> m BDD
nithVar var = liftM (BDD . xor c_sylvanComplement) $ unsafePrimToPrim $ c_ithVar (fromIntegral var)

foreign import ccall safe "mtbdd_ref"
    c_ref :: CBDD -> IO (CBDD)

ref :: PrimMonad m => BDD -> m BDD
ref (BDD bdd) = liftM BDD $ unsafePrimToPrim $ c_ref bdd

refMap :: PrimMonad m => BDDMap -> m BDDMap
refMap (BDDMap bdd) = liftM BDDMap $ unsafePrimToPrim $ c_ref bdd

foreign import ccall safe "mtbdd_deref"
    c_deref :: CBDD -> IO ()

deref :: PrimMonad m => BDD -> m ()
deref (BDD bdd) = unsafePrimToPrim $ c_deref bdd

derefMap :: PrimMonad m => BDDMap -> m ()
derefMap (BDDMap bdd) = unsafePrimToPrim $ c_deref bdd

foreign import ccall safe "sylvan_gc_stub"
    c_gc :: IO ()

gc :: PrimMonad m => m ()
gc = unsafePrimToPrim $ c_gc

foreign import ccall safe "sylvan_gc_enable"
    c_gcEnable :: IO ()

gcEnable :: PrimMonad m => m ()
gcEnable = unsafePrimToPrim c_gcEnable

foreign import ccall safe "sylvan_gc_disable"
    c_gcDisable :: IO ()

gcDisable :: PrimMonad m => m ()
gcDisable = unsafePrimToPrim c_gcDisable

neg :: BDD -> BDD
neg (BDD x) = BDD $ xor c_sylvanComplement x

foreign import ccall safe "sylvan_ite_stub"
    c_ite :: CBDD -> CBDD -> CBDD -> IO CBDD

ite :: PrimMonad m => BDD -> BDD -> BDD -> m BDD
ite (BDD a) (BDD b) (BDD c) = liftM BDD $ unsafePrimToPrim $ c_ite a b c

foreign import ccall safe "sylvan_xor_stub"
    c_xor :: CBDD -> CBDD -> IO CBDD

bxor :: PrimMonad m => BDD -> BDD -> m BDD
bxor (BDD a) (BDD b) = liftM BDD $ unsafePrimToPrim $ c_xor a b

bequiv :: PrimMonad m => BDD -> BDD -> m BDD
bequiv a b = liftM neg $ bxor a b

foreign import ccall safe "sylvan_and_stub"
    c_and :: CBDD -> CBDD -> IO CBDD

band :: PrimMonad m => BDD -> BDD -> m BDD
band (BDD a) (BDD b) = liftM BDD $ unsafePrimToPrim $ c_and a b

bor :: PrimMonad m => BDD -> BDD -> m BDD
bor a b = liftM neg $ band (neg a) (neg b)

bnand :: PrimMonad m => BDD -> BDD -> m BDD
bnand a b = liftM neg $ band a b

bnor :: PrimMonad m => BDD -> BDD -> m BDD
bnor a b = liftM neg $ bor a b

bimp :: PrimMonad m => BDD -> BDD -> m BDD
bimp a b = liftM neg $ band a (neg b)

bimpinv :: PrimMonad m => BDD -> BDD -> m BDD
bimpinv a b = liftM neg $ band (neg a) b

biimp :: PrimMonad m => BDD -> BDD -> m BDD
biimp = bequiv

diff :: PrimMonad m => BDD -> BDD -> m BDD
diff a b = band a (neg b)

less :: PrimMonad m => BDD -> BDD -> m BDD
less a b = band (neg a) b

foreign import ccall safe "sylvan_exists_stub"
    c_exists :: CBDD -> CBDD -> IO CBDD

exists :: PrimMonad m => BDD -> BDD -> m BDD
exists (BDD a) (BDD variables) = liftM BDD $ unsafePrimToPrim $ c_exists a variables

forall :: PrimMonad m => BDD -> BDD -> m BDD
forall a variables = liftM neg $ exists (neg a) variables

foreign import ccall safe "sylvan_and_exists_stub"
    c_and_exists :: CBDD -> CBDD -> CBDD -> IO CBDD

andExists :: PrimMonad m => BDD -> BDD -> BDD -> m BDD
andExists (BDD a) (BDD b) (BDD vars) = liftM BDD $ unsafePrimToPrim $ c_and_exists a b vars

foreign import ccall safe "mtbdd_set_from_array"
    c_setFromArray :: Ptr CBDDVar -> CSize -> IO CBDD

setFromArray :: PrimMonad m => [BDDVar] -> m BDD
setFromArray vars = liftM BDD $ unsafePrimToPrim $ 
    withArrayLen (map fromIntegral vars) $ \l p -> 
        c_setFromArray p (fromIntegral l)

mapEmpty :: BDDMap
mapEmpty = BDDMap c_sylvanFalse 

foreign import ccall safe "mtbdd_map_add"
    c_mapAdd :: CBDDMap -> CBDDVar -> CBDD -> IO CBDDMap

mapAdd :: PrimMonad m => BDDMap -> BDDVar -> BDD -> m BDDMap
mapAdd (BDDMap m) var (BDD x) = liftM BDDMap $ unsafePrimToPrim $ c_mapAdd m (fromIntegral var) x

foreign import ccall safe "sylvan_compose_stub"
    c_compose :: CBDD -> CBDDMap -> IO CBDD

compose :: PrimMonad m => BDD -> BDDMap -> m BDD
compose (BDD f) (BDDMap m) = liftM BDD $ unsafePrimToPrim $ c_compose f m

foreign import ccall safe "sylvan_set_reorder_maxswap"
    c_sylvanSetReorderMaxSwap :: CInt -> IO ()

setReorderMaxSwap :: PrimMonad m => Int -> m ()
setReorderMaxSwap maxSwap  = unsafePrimToPrim $ c_sylvanSetReorderMaxSwap (fromIntegral maxSwap)

foreign import ccall safe "sylvan_set_reorder_maxgrowth"
    c_sylvanSetReorderMaxGrowth :: CFloat -> IO ()

setReorderMaxGrowth :: PrimMonad m => Rational -> m ()
setReorderMaxGrowth maxGrowth  = unsafePrimToPrim $ c_sylvanSetReorderMaxGrowth (fromRational maxGrowth)

foreign import ccall safe "sylvan_set_reorder_maxvar"
    c_sylvanSetReorderMaxVar :: CInt -> IO ()

setReorderMaxVar :: PrimMonad m => Int -> m ()
setReorderMaxVar maxVar  = unsafePrimToPrim $ c_sylvanSetReorderMaxVar (fromIntegral maxVar)

foreign import ccall safe "sylvan_set_reorder_nodes_threshold"
    c_sylvanSetReorderNodesThreshold :: CInt -> IO ()

setReorderNodesThreshold:: PrimMonad m => Int -> m ()
setReorderNodesThreshold threshold  = unsafePrimToPrim $ c_sylvanSetReorderNodesThreshold (fromIntegral threshold)

foreign import ccall safe "sylvan_set_reorder_timelimit_sec"
    c_sylvanSetReorderTimeLimitSec :: CDouble -> IO ()

setReorderTimeLimitSec :: PrimMonad m => Rational -> m ()
setReorderTimeLimitSec timeLimit  = unsafePrimToPrim $ c_sylvanSetReorderTimeLimitSec (fromRational timeLimit)

foreign import ccall safe "sylvan_reduce_heap_bounded"
    c_sylvanReduceHeap :: IO ()

--Reorder right now
reduceHeap :: PrimMonad m => m ()
reduceHeap = unsafePrimToPrim c_sylvanReduceHeap 

foreign import ccall safe "sylvan_test_reduce_heap"
    c_sylvanTestReduceHeap :: IO ()

--Reorder right now if needed
testReduceHeap:: PrimMonad m => m ()
testReduceHeap  = unsafePrimToPrim c_sylvanTestReduceHeap 


----TODO: doesnt seem to exist
--foreign import ccall safe "sylvan_report_stats"
--    c_reportStats :: IO ()
--
--reportStats :: PrimMonad m => m ()
--reportStats = unsafePrimToPrim c_reportStats
--
--foreign import ccall safe "sylvan_printdot"
--    c_printDot :: CBDD -> IO ()
--
--printDot :: PrimMonad m => BDD -> m ()
--printDot (BDD x) = unsafePrimToPrim $ c_printDot x
--
----TODO: a macro
--foreign import ccall safe "sylvan_print"
--    c_bddPrint :: CBDD -> IO ()
--
--bddPrint :: PrimMonad m => BDD -> m ()
--bddPrint (BDD x) = unsafePrimToPrim $ c_bddPrint x
--
----TODO: a macro
--foreign import ccall safe "sylvan_printsha"
--    c_printSHA :: CBDD -> IO ()
--
--printSHA :: BDD -> IO ()
--printSHA (BDD x) = unsafePrimToPrim $ c_printSHA x

foreign import ccall safe "sylvan_cube"
    c_cube :: CBDD -> Ptr CUChar -> IO CBDD

data Polarity = 
      Negative
    | Positive
    | DontCare
    deriving (Show)

polarityToInt :: Integral i => Polarity -> i
polarityToInt Negative = 0
polarityToInt Positive = 1
polarityToInt DontCare = 2

cube :: PrimMonad m => BDD -> [Polarity] -> m BDD
cube (BDD vars) polarities = liftM BDD $ unsafePrimToPrim $ 
    withArrayLen (map polarityToInt polarities) $ \_ pp -> 
        c_cube vars pp
