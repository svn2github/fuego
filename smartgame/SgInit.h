//----------------------------------------------------------------------------
/** @file SgInit.h
    Initialization of the SmartGo module.
*/
//----------------------------------------------------------------------------

#ifndef SG_INIT_H
#define SG_INIT_H

//----------------------------------------------------------------------------

/** Call all lower-level Fini functions.
    This function must be called after using the SmartGo module.
    Also calls SgMemCheck.
    @note Will become obsolete in the future
    @see SgInit
*/
void SgFini();

/** Call all lower-level initialization functions.
    This function must be called before using the SmartGo module.
    Returns false if one of them returns false.
    @note Don't add any more global variables that need explicit
    initialization; this function will become unnecessary in the future.
    Currently still needed for:
    - Property
    @throws SgException on error
*/
void SgInit();

/** Check that SgInit was called.
    @throws SgException if not
*/
void SgInitCheck();

//----------------------------------------------------------------------------

#endif // SG_INIT_H

