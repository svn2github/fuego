//----------------------------------------------------------------------------
/** @file SgMemCheck.h
    Memory leak checks.
*/
//----------------------------------------------------------------------------

#ifndef SGMEMCHECK_H
#define SGMEMCHECK_H

//----------------------------------------------------------------------------

/** Check for memory leaks.
    Checks allocation counters of classes that support it with an assertion.
    Call this at the end of your program, when all class instances should
    have been destructed.
*/
void SgMemCheck();

//----------------------------------------------------------------------------

#endif // SGMEMCHECK_H

