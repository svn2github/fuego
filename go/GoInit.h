//----------------------------------------------------------------------------
/** @file GoInit.h
    Initialization of module Go.
*/
//----------------------------------------------------------------------------

#ifndef GOINIT_H
#define GOINIT_H

//----------------------------------------------------------------------------

void GoFini();

/** Initialization of module Go.
    Must be called after SgInit.
    @throws SgException on failure.
*/
void GoInit();

void GoInitCheck();

//----------------------------------------------------------------------------

#endif // GOINIT_H

