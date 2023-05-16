{-# LANGUAGE RecordWildCards #-}
module BDDCudd where

import BDD

import Control.Monad.ST
import Control.Monad.ST.Unsafe

import qualified Cudd.Imperative as Cudd
import Cudd.Imperative (DDManager, DDNode)
import Cudd.Reorder

constructOps :: DDManager s u -> Ops s (DDNode s u)
constructOps m = Ops {..}
    where
    bAnd              = Cudd.bAnd m
    bOr               = Cudd.bOr  m
    lEq               = Cudd.lEq  m
    neg               = Cudd.bNot
    vectorCompose     = Cudd.vectorCompose m
    newVar            = Cudd.newVar m
    computeCube       = Cudd.nodesToCube m
    computeCube2      = Cudd.computeCube m
    getSize           = Cudd.readSize m
    ithVar            = Cudd.ithVar m
    bforall           = flip $ Cudd.bForall m
    bexists           = flip $ Cudd.bExists m
    deref             = Cudd.deref m
    ref               = Cudd.ref
    btrue             = Cudd.bOne m
    bfalse            = Cudd.bZero m
    andAbstract c x y = Cudd.andAbstract m x y c
    getIdx            = Cudd.readIndex
    largestCube       = Cudd.largestCube m
    makePrime         = Cudd.makePrime m
    supportIndices    = Cudd.supportIndices m
    printMinterm      = Cudd.printMinterm m