/**
  @file 

  @ingroup cudd

  @brief Quantification functions for ADDs.

  @author Fabio Somenzi

  @copyright@parblock
  Copyright (c) 1995-2015, Regents of the University of Colorado

  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  Neither the name of the University of Colorado nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
  @endparblock

*/

#include "util.h"
#include "cuddInt.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Stucture declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/** \cond */

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static int addCheckPositiveCube (DdManager *manager, DdNode *cube);

static int addCheckWeightedCube (DdManager *manager, DdNode *cube);

/** \endcond */


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**
  @brief Existentially Abstracts all the variables in cube from f.

  @details Abstracts all the variables in cube from f by summing
  over all possible values taken by the variables.

  @return the abstracted %ADD.

  @sideeffect None

  @see Cudd_addUnivAbstract Cudd_bddExistAbstract
  Cudd_addOrAbstract

*/
DdNode *
Cudd_addExistAbstract(
  DdManager * manager,
  DdNode * f,
  DdNode * cube)
{
    DdNode *res;

    if (addCheckPositiveCube(manager, cube) == 0) {
        (void) fprintf(manager->err,"Error: Can only abstract cubes");
        return(NULL);
    }

    do {
	manager->reordered = 0;
	res = cuddAddExistAbstractRecur(manager, f, cube);
    } while (manager->reordered == 1);
    if (manager->errorCode == CUDD_TIMEOUT_EXPIRED && manager->timeoutHandler) {
        manager->timeoutHandler(manager, manager->tohArg);
    }

    return(res);

} /* end of Cudd_addExistAbstract */


DdNode *
Cudd_addWeightedAbstract(
  DdManager * manager,
  DdNode * f,
  DdNode * cube,
  long double (*getNegWt)(long long),
  DD_WAOP op,
  DdNode* opZero)
{
    DdNode *res;

    if (addCheckPositiveCube(manager, cube) == 0) {
        (void) fprintf(manager->err,"Error: Cudd_addWeightedExistAbstract can only abstract weighted cubes");
        return(NULL);
    }

    do {
	manager->reordered = 0;
    // (void) fprintf(manager->err,"Starting WEARecur..\n");
	res = cuddAddWeightedAbstractRecur(manager, f, cube, getNegWt, op, opZero);
    } while (manager->reordered == 1);
    if (manager->errorCode == CUDD_TIMEOUT_EXPIRED && manager->timeoutHandler) {
        manager->timeoutHandler(manager, manager->tohArg);
    }

    return(res);

} /* end of Cudd_addWeightedExistAbstract */

/**
  @brief Existentially Abstracts all the variables in cube from f.

  @details Abstracts all the variables in cube from f by summing
  over all possible values taken by the variables.

  @return the abstracted %ADD.

  @sideeffect None

  @see Cudd_addUnivAbstract Cudd_bddExistAbstract
  Cudd_addOrAbstract

*/
DdNode *
Cudd_addLogSumExistAbstract(
  DdManager * manager,
  DdNode * f,
  DdNode * cube)
{
    DdNode *res;

    if (addCheckPositiveCube(manager, cube) == 0) {
        (void) fprintf(manager->err,"Error: Can only abstract cubes");
        return(NULL);
    }

    do {
	manager->reordered = 0;
	res = cuddAddLogSumExistAbstractRecur(manager, f, cube);
    } while (manager->reordered == 1);
    if (manager->errorCode == CUDD_TIMEOUT_EXPIRED && manager->timeoutHandler) {
        manager->timeoutHandler(manager, manager->tohArg);
    }

    return(res);

} /* end of Cudd_addExistAbstract */


/**
  @brief Universally Abstracts all the variables in cube from f.

  @details Abstracts all the variables in cube from f by taking
  the product over all possible values taken by the variable.

  @return the abstracted %ADD if successful; NULL otherwise.

  @sideeffect None

  @see Cudd_addExistAbstract Cudd_bddUnivAbstract
  Cudd_addOrAbstract

*/
DdNode *
Cudd_addUnivAbstract(
  DdManager * manager,
  DdNode * f,
  DdNode * cube)
{
    DdNode		*res;

    if (addCheckPositiveCube(manager, cube) == 0) {
	(void) fprintf(manager->err,"Error:  Can only abstract cubes");
	return(NULL);
    }

    do {
	manager->reordered = 0;
	res = cuddAddUnivAbstractRecur(manager, f, cube);
    } while (manager->reordered == 1);
    if (manager->errorCode == CUDD_TIMEOUT_EXPIRED && manager->timeoutHandler) {
        manager->timeoutHandler(manager, manager->tohArg);
    }

    return(res);

} /* end of Cudd_addUnivAbstract */


/**
  @brief Disjunctively abstracts all the variables in cube from the
  0-1 %ADD f.

  @details Abstracts all the variables in cube from the 0-1 %ADD f
  by taking the disjunction over all possible values taken by the
  variables.

  @return the abstracted %ADD if successful; NULL otherwise.

  @sideeffect None

  @see Cudd_addUnivAbstract Cudd_addExistAbstract

*/
DdNode *
Cudd_addOrAbstract(
  DdManager * manager,
  DdNode * f,
  DdNode * cube)
{
    DdNode *res;

    if (addCheckPositiveCube(manager, cube) == 0) {
        (void) fprintf(manager->err,"Error: Can only abstract cubes");
        return(NULL);
    }

    do {
	manager->reordered = 0;
	res = cuddAddOrAbstractRecur(manager, f, cube);
    } while (manager->reordered == 1);
    if (manager->errorCode == CUDD_TIMEOUT_EXPIRED && manager->timeoutHandler) {
        manager->timeoutHandler(manager, manager->tohArg);
    }
    return(res);

} /* end of Cudd_addOrAbstract */


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/**
  @brief Performs the recursive step of Cudd_addExistAbstract.

  @details Returns the %ADD obtained by abstracting the variables of
  cube from f, if successful; NULL otherwise.

  @sideeffect None

*/
DdNode *
cuddAddExistAbstractRecur(
  DdManager * manager,
  DdNode * f,
  DdNode * cube)
{
    DdNode	*T, *E, *res, *res1, *res2, *zero;

    statLine(manager);
    zero = DD_ZERO(manager);

    /* Cube is guaranteed to be a cube at this point. */	
    if (f == zero || cuddIsConstant(cube)) {  
        return(f);
    }

    /* Abstract a variable that does not appear in f => multiply by 2. */
    if (cuddI(manager,f->index) > cuddI(manager,cube->index)) {
	res1 = cuddAddExistAbstractRecur(manager, f, cuddT(cube));
	if (res1 == NULL) return(NULL);
	cuddRef(res1);
	/* Use the "internal" procedure to be alerted in case of
	** dynamic reordering. If dynamic reordering occurs, we
	** have to abort the entire abstraction.
	*/
	res = cuddAddApplyRecur(manager,Cudd_addPlus,res1,res1);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
	cuddRef(res);
	Cudd_RecursiveDeref(manager,res1);
	cuddDeref(res);
        return(res);
    }

    if ((res = cuddCacheLookup2(manager, Cudd_addExistAbstract, f, cube)) != NULL) {
	return(res);
    }

    checkWhetherToGiveUp(manager);

    T = cuddT(f);
    E = cuddE(f);

    /* If the two indices are the same, so are their levels. */
    if (f->index == cube->index) {
	res1 = cuddAddExistAbstractRecur(manager, T, cuddT(cube));
	if (res1 == NULL) return(NULL);
        cuddRef(res1);
	res2 = cuddAddExistAbstractRecur(manager, E, cuddT(cube));
	if (res2 == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
        cuddRef(res2);
	res = cuddAddApplyRecur(manager, Cudd_addPlus, res1, res2);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    Cudd_RecursiveDeref(manager,res2);
	    return(NULL);
	}
	cuddRef(res);
	Cudd_RecursiveDeref(manager,res1);
	Cudd_RecursiveDeref(manager,res2);
	cuddCacheInsert2(manager, Cudd_addExistAbstract, f, cube, res);
	cuddDeref(res);
        return(res);
    } else { /* if (cuddI(manager,f->index) < cuddI(manager,cube->index)) */
	res1 = cuddAddExistAbstractRecur(manager, T, cube);
	if (res1 == NULL) return(NULL);
        cuddRef(res1);
	res2 = cuddAddExistAbstractRecur(manager, E, cube);
	if (res2 == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
        cuddRef(res2);
	res = (res1 == res2) ? res1 :
	    cuddUniqueInter(manager, (int) f->index, res1, res2);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    Cudd_RecursiveDeref(manager,res2);
	    return(NULL);
	}
	cuddDeref(res1);
	cuddDeref(res2);
	cuddCacheInsert2(manager, Cudd_addExistAbstract, f, cube, res);
        return(res);
    }	    

} /* end of cuddAddExistAbstractRecur */


/**
  @brief Performs the recursive step of Cudd_addWeightedExistAbstract.

  @details Returns the %ADD obtained by abstracting the variables of
  weighted cube from f, if successful; NULL otherwise.

  @sideeffect None

*/
DdNode *
cuddAddWeightedAbstractRecur(
  DdManager * manager,
  DdNode * f,
  DdNode * cube,
  long double (*getNegWt)(long long),
  DD_WAOP op,
  DdNode* opZero)
{
    DdNode	*T, *E, *res, *res1, *res2; // *zero;

    DdNode *scalar;

    statLine(manager);
    // zero = DD_ZERO(manager);

    /* weightedCube is guaranteed to be a weighted cube at this point. */	
    if (f == opZero || cuddIsConstant(cube)) {  
        assert(f == opZero || (cube == DD_ONE(manager)));
        // if (f==zero) fprintf(manager->err,"f is zero!\n");
        return(f);
    }

    /* Abstract a variable that does not appear in f => multiply by 2 if variable is unweighted otherwise multiply by 1 since weights sum to 1 (i.e. do nothing). */
    if (cuddI(manager,f->index) > cuddI(manager,cube->index)) {
        // (void) fprintf(manager->err,"WEARecur cond 1..\n");
    	res1 = cuddAddWeightedAbstractRecur(manager, f, cuddT(cube), getNegWt, op, opZero);
        if (res1 == NULL) return(NULL);
        cuddRef(res1);
        /* Use the "internal" procedure to be alerted in case of
        ** dynamic reordering. If dynamic reordering occurs, we
        ** have to abort the entire abstraction.
        */

        long double negWt = (*getNegWt)(cube->index);
        scalar = cuddUniqueConst(manager,negWt);
        cuddRef(scalar);    
        res = cuddAddWeightedApplyRecur(manager,op,res1,res1, scalar);
        if (res == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            return(NULL);
        }
        cuddRef(res);
        Cudd_RecursiveDeref(manager,res1);
        cuddDeref(res);
        return(res);
    }

    if ((res = cuddCacheLookup(manager, getTag(cuddAddWeightedAbstractRecur,op), f, cube, opZero)) != NULL) {
	return(res);
    }

    checkWhetherToGiveUp(manager);

    T = cuddT(f);
    E = cuddE(f);

    /* If the two indices are the same, so are their levels. */
    if (f->index == cube->index) {
        // (void) fprintf(manager->err,"WEARecur cond 2..\n");
        res1 = cuddAddWeightedAbstractRecur(manager, T, cuddT(cube), getNegWt, op, opZero);
        if (res1 == NULL) return(NULL);
            cuddRef(res1);
        res2 = cuddAddWeightedAbstractRecur(manager, E, cuddT(cube), getNegWt, op, opZero);
        if (res2 == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            return(NULL);
        }
        cuddRef(res2);
        // if (cuddE(weightedCube) == zero) { /* unweighted var so do plain addplus */
        //     res = cuddAddWeightedApplyRecur(manager, Cudd_addWeightedPlus, res1, res2, DD_ZERO(manager));
        // } else {
        //     scalar = cuddE(weightedCube);
        //     cuddRef(scalar);
        //     res = cuddAddWeightedApplyRecur(manager, Cudd_addWeightedPlus, res1, res2, scalar);
        //     cuddDeref(scalar);
        // }
        long double negWt = (*getNegWt)(cube->index);
        scalar = cuddUniqueConst(manager,negWt);
        cuddRef(scalar);
        res = cuddAddWeightedApplyRecur(manager, op, res1, res2, scalar);
        if (res == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            Cudd_RecursiveDeref(manager,res2);
            return(NULL);
        }
        cuddRef(res);
        Cudd_RecursiveDeref(manager,res1);
        Cudd_RecursiveDeref(manager,res2);
        cuddCacheInsert(manager, getTag(cuddAddWeightedAbstractRecur,op), f, cube, opZero, res);
        // cuddDeref(scalar);  //gives error if uncommented
        cuddDeref(res);
        return(res);
    } else { /* if (cuddI(manager,f->index) < cuddI(manager,cube->index)) */
        // (void) fprintf(manager->err,"WEARecur cond 3..\n");
        res1 = cuddAddWeightedAbstractRecur(manager, T, cube, getNegWt, op, opZero);
        if (res1 == NULL) return(NULL);
            cuddRef(res1);
        res2 = cuddAddWeightedAbstractRecur(manager, E, cube, getNegWt, op, opZero);
        if (res2 == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            return(NULL);
        }
            cuddRef(res2);
        res = (res1 == res2) ? res1 :
            cuddUniqueInter(manager, (int) f->index, res1, res2);
        if (res == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            Cudd_RecursiveDeref(manager,res2);
            return(NULL);
        }
        cuddDeref(res1);
        cuddDeref(res2);
        cuddCacheInsert(manager, getTag(cuddAddWeightedAbstractRecur,op), f, cube, opZero, res);
        return(res);
    }	    

} /* end of cuddAddWeightedExistAbstractRecur */

/**
  @brief Performs the recursive step of Cudd_addExistAbstract.

  @details Returns the %ADD obtained by abstracting the variables of
  cube from f, if successful; NULL otherwise.

  @sideeffect None

*/
DdNode *
cuddAddLogSumExistAbstractRecur(
  DdManager * manager,
  DdNode * f,
  DdNode * cube)
{
    DdNode	*T, *E, *res, *res1, *res2, *zero;

    statLine(manager);
    zero = DD_MINUS_INFINITY(manager); // since zero in log case is minus infinity

    /* Cube is guaranteed to be a cube at this point. */	
    if (f == zero || cuddIsConstant(cube)) {  
        return(f);
    }

    /* Abstract a variable that does not appear in f => multiply by 2. */
    if (cuddI(manager,f->index) > cuddI(manager,cube->index)) {
	res1 = cuddAddLogSumExistAbstractRecur(manager, f, cuddT(cube));
	if (res1 == NULL) return(NULL);
	cuddRef(res1);
	/* Use the "internal" procedure to be alerted in case of
	** dynamic reordering. If dynamic reordering occurs, we
	** have to abort the entire abstraction.
	*/
	res = cuddAddApplyRecur(manager,Cudd_addLogSumExp,res1,res1);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
	cuddRef(res);
	Cudd_RecursiveDeref(manager,res1);
	cuddDeref(res);
        return(res);
    }

    if ((res = cuddCacheLookup2(manager, Cudd_addLogSumExistAbstract, f, cube)) != NULL) {
	return(res);
    }

    checkWhetherToGiveUp(manager);

    T = cuddT(f);
    E = cuddE(f);

    /* If the two indices are the same, so are their levels. */
    if (f->index == cube->index) {
	res1 = cuddAddLogSumExistAbstractRecur(manager, T, cuddT(cube));
	if (res1 == NULL) return(NULL);
        cuddRef(res1);
	res2 = cuddAddLogSumExistAbstractRecur(manager, E, cuddT(cube));
	if (res2 == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
        cuddRef(res2);
	res = cuddAddApplyRecur(manager, Cudd_addLogSumExp, res1, res2);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    Cudd_RecursiveDeref(manager,res2);
	    return(NULL);
	}
	cuddRef(res);
	Cudd_RecursiveDeref(manager,res1);
	Cudd_RecursiveDeref(manager,res2);
	cuddCacheInsert2(manager, Cudd_addLogSumExistAbstract, f, cube, res);
	cuddDeref(res);
        return(res);
    } else { /* if (cuddI(manager,f->index) < cuddI(manager,cube->index)) */
	res1 = cuddAddLogSumExistAbstractRecur(manager, T, cube);
	if (res1 == NULL) return(NULL);
        cuddRef(res1);
	res2 = cuddAddLogSumExistAbstractRecur(manager, E, cube);
	if (res2 == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
        cuddRef(res2);
	res = (res1 == res2) ? res1 :
	    cuddUniqueInter(manager, (int) f->index, res1, res2);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    Cudd_RecursiveDeref(manager,res2);
	    return(NULL);
	}
	cuddDeref(res1);
	cuddDeref(res2);
	cuddCacheInsert2(manager, Cudd_addLogSumExistAbstract, f, cube, res);
        return(res);
    }	    

} /* end of cuddAddExistAbstractRecur */


/**
  @brief Performs the recursive step of Cudd_addUnivAbstract.

  @return the %ADD obtained by abstracting the variables of cube from
  f, if successful; NULL otherwise.

  @sideeffect None

*/
DdNode *
cuddAddUnivAbstractRecur(
  DdManager * manager,
  DdNode * f,
  DdNode * cube)
{
    DdNode	*T, *E, *res, *res1, *res2, *one, *zero;

    statLine(manager);
    one = DD_ONE(manager);
    zero = DD_ZERO(manager);

    /* Cube is guaranteed to be a cube at this point.
    ** zero and one are the only constatnts c such that c*c=c.
    */
    if (f == zero || f == one || cube == one) {  
	return(f);
    }

    /* Abstract a variable that does not appear in f. */
    if (cuddI(manager,f->index) > cuddI(manager,cube->index)) {
	res1 = cuddAddUnivAbstractRecur(manager, f, cuddT(cube));
	if (res1 == NULL) return(NULL);
	cuddRef(res1);
	/* Use the "internal" procedure to be alerted in case of
	** dynamic reordering. If dynamic reordering occurs, we
	** have to abort the entire abstraction.
	*/
	res = cuddAddApplyRecur(manager, Cudd_addTimes, res1, res1);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
	cuddRef(res);
	Cudd_RecursiveDeref(manager,res1);
	cuddDeref(res);
	return(res);
    }

    if ((res = cuddCacheLookup2(manager, Cudd_addUnivAbstract, f, cube)) != NULL) {
	return(res);
    }

    checkWhetherToGiveUp(manager);

    T = cuddT(f);
    E = cuddE(f);

    /* If the two indices are the same, so are their levels. */
    if (f->index == cube->index) {
	res1 = cuddAddUnivAbstractRecur(manager, T, cuddT(cube));
	if (res1 == NULL) return(NULL);
        cuddRef(res1);
	res2 = cuddAddUnivAbstractRecur(manager, E, cuddT(cube));
	if (res2 == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
        cuddRef(res2);
	res = cuddAddApplyRecur(manager, Cudd_addTimes, res1, res2);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    Cudd_RecursiveDeref(manager,res2);
	    return(NULL);
	}
	cuddRef(res);
	Cudd_RecursiveDeref(manager,res1);
	Cudd_RecursiveDeref(manager,res2);
	cuddCacheInsert2(manager, Cudd_addUnivAbstract, f, cube, res);
	cuddDeref(res);
        return(res);
    } else { /* if (cuddI(manager,f->index) < cuddI(manager,cube->index)) */
	res1 = cuddAddUnivAbstractRecur(manager, T, cube);
	if (res1 == NULL) return(NULL);
        cuddRef(res1);
	res2 = cuddAddUnivAbstractRecur(manager, E, cube);
	if (res2 == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
        cuddRef(res2);
	res = (res1 == res2) ? res1 :
	    cuddUniqueInter(manager, (int) f->index, res1, res2);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    Cudd_RecursiveDeref(manager,res2);
	    return(NULL);
	}
	cuddDeref(res1);
	cuddDeref(res2);
	cuddCacheInsert2(manager, Cudd_addUnivAbstract, f, cube, res);
        return(res);
    }

} /* end of cuddAddUnivAbstractRecur */


/**
  @brief Performs the recursive step of Cudd_addOrAbstract.

  @return the %ADD obtained by abstracting the variables of cube from
  f, if successful; NULL otherwise.

  @sideeffect None

*/
DdNode *
cuddAddOrAbstractRecur(
  DdManager * manager,
  DdNode * f,
  DdNode * cube)
{
    DdNode	*T, *E, *res, *res1, *res2, *one;

    statLine(manager);
    one = DD_ONE(manager);

    /* Cube is guaranteed to be a cube at this point. */
    if (cuddIsConstant(f) || cube == one) {  
	return(f);
    }

    /* Abstract a variable that does not appear in f. */
    if (cuddI(manager,f->index) > cuddI(manager,cube->index)) {
	res = cuddAddOrAbstractRecur(manager, f, cuddT(cube));
	return(res);
    }

    if ((res = cuddCacheLookup2(manager, Cudd_addOrAbstract, f, cube)) != NULL) {
	return(res);
    }

    checkWhetherToGiveUp(manager);

    T = cuddT(f);
    E = cuddE(f);

    /* If the two indices are the same, so are their levels. */
    if (f->index == cube->index) {
	res1 = cuddAddOrAbstractRecur(manager, T, cuddT(cube));
	if (res1 == NULL) return(NULL);
        cuddRef(res1);
	if (res1 != one) {
	    res2 = cuddAddOrAbstractRecur(manager, E, cuddT(cube));
	    if (res2 == NULL) {
		Cudd_RecursiveDeref(manager,res1);
		return(NULL);
	    }
	    cuddRef(res2);
	    res = cuddAddApplyRecur(manager, Cudd_addOr, res1, res2);
	    if (res == NULL) {
		Cudd_RecursiveDeref(manager,res1);
		Cudd_RecursiveDeref(manager,res2);
		return(NULL);
	    }
	    cuddRef(res);
	    Cudd_RecursiveDeref(manager,res1);
	    Cudd_RecursiveDeref(manager,res2);
	} else {
	    res = res1;
	}
	cuddCacheInsert2(manager, Cudd_addOrAbstract, f, cube, res);
	cuddDeref(res);
        return(res);
    } else { /* if (cuddI(manager,f->index) < cuddI(manager,cube->index)) */
	res1 = cuddAddOrAbstractRecur(manager, T, cube);
	if (res1 == NULL) return(NULL);
        cuddRef(res1);
	res2 = cuddAddOrAbstractRecur(manager, E, cube);
	if (res2 == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
        cuddRef(res2);
	res = (res1 == res2) ? res1 :
	    cuddUniqueInter(manager, (int) f->index, res1, res2);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    Cudd_RecursiveDeref(manager,res2);
	    return(NULL);
	}
	cuddDeref(res1);
	cuddDeref(res2);
	cuddCacheInsert2(manager, Cudd_addOrAbstract, f, cube, res);
        return(res);
    }

} /* end of cuddAddOrAbstractRecur */



/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/


/**
  @brief Checks whether cube is an %ADD representing the product
  of positive literals.

  @return 1 in case of success; 0 otherwise.

  @sideeffect None

*/
static int
addCheckPositiveCube(
  DdManager * manager,
  DdNode * cube)
{
    if (Cudd_IsComplement(cube)) return(0);
    if (cube == DD_ONE(manager)) return(1);
    if (cuddIsConstant(cube)) return(0);
    if (cuddE(cube) == DD_ZERO(manager)) {
        return(addCheckPositiveCube(manager, cuddT(cube)));
    }
    return(0);

} /* end of addCheckPositiveCube */


/* Does not check for valid constant values of Else edge of vars */
static int
addCheckWeightedCube(
  DdManager * manager,
  DdNode * cube)
{
    if (Cudd_IsComplement(cube)) {
        fprintf(manager->err,"complement in weightedcube\n");
        return(0);
    }
    if (cube == DD_ONE(manager)) return(1);
    if (cuddIsConstant(cube)) {
        fprintf(manager->err,"non-1 constant in weightedcube\n");
        return(0);
    }
    if (cuddIsConstant(cuddE(cube))) {
        return(addCheckWeightedCube(manager, cuddT(cube)));
    }
    fprintf(manager->err,"none of the above in weightedcube. index of else is %d\n",cuddE(cube)->index);
    return(0);

} /* end of addCheckPositiveCube */
