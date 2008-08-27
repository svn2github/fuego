//----------------------------------------------------------------------------
/** @file SgSystem.h
    System specific definitions for SmartGo.

    This file contains system specific defines and includes.
    It always needs to be the first header file included by any .cpp file.
 */
//----------------------------------------------------------------------------

#ifndef SG_SYSTEM_H
#define SG_SYSTEM_H

//----------------------------------------------------------------------------

// Used by GNU Autotools
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

//----------------------------------------------------------------------------

// Check which OS we use. Some stuff such as I/O, UI or profiling depends
// on this if one of the symbols is already predefined by a compile switch
// or by a previous header file, then leave it as it is.

#ifdef MAC
    #define UNIX 0
#else
    #ifdef UNIX
        #define MAC 0
    #else
        #error "no OS defined"
    #endif
#endif

//----------------------------------------------------------------------------

/** Avoid compiler warnings for unused variables.
    This function is more portable than using a \#pragma directive.
*/
template <class T>
inline void SG_UNUSED(const T&)
{
}

/** Avoid compiler warnings for variables used only if _DEBUG is defined.
    This macro is more portable than using a \#pragma directive.
*/
#ifdef _DEBUG
#define SG_DEBUG_ONLY(x)
#else
#define SG_DEBUG_ONLY(x) SG_UNUSED(x)
#endif

//----------------------------------------------------------------------------

#ifdef __GNUC__
#define SG_ATTR_ALWAYS_INLINE __attribute__((always_inline))
#define SG_ATTR_NOINLINE __attribute__((noinline))
#else
#define SG_ATTR_NOINLINE
#define SG_ATTR_ALWAYS_INLINE
#endif

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1)
#define SG_ATTR_FLATTEN __attribute__((flatten))
#else
#define SG_ATTR_FLATTEN
#endif

//----------------------------------------------------------------------------

#if UNIX
#include <sys/types.h>
#if (BYTE_ORDER == BIG_ENDIAN)
#define OTHER_BYTE_ORDER 0
#else
#define OTHER_BYTE_ORDER 1
#endif
#endif
#if MAC
#define OTHER_BYTE_ORDER 0
#endif

//----------------------------------------------------------------------------

/** Additional code to run in debug mode after an assertion failed. */
class SgAssertionHandler
{
public:
    /** Constructor.
        Automatically registers the handler.
    */
    SgAssertionHandler();

    /** Constructor.
        Automatically unregisters the handler.
    */
    virtual ~SgAssertionHandler();

    virtual void Run() = 0;
};

#ifdef _DEBUG

/** System-specific action when an ASSERT fails */
void SgHandleAssertion(const char* expr, const char* file, int line);

#define SG_ASSERT(x) \
    do \
    { \
        if(! (x)) \
            ::SgHandleAssertion(#x, __FILE__, __LINE__); \
    } while (false)
#else
#define SG_ASSERT(x) (static_cast<void>(0))
#endif

#define SG_ASSERTRANGE(i, from, to) SG_ASSERT(i >= from && i <= to)

//----------------------------------------------------------------------------

#if _DEBUG
const bool SG_CHECK = true;
const bool SG_HEAVYCHECK = SG_CHECK && true;
#else
const bool SG_CHECK = false;
const bool SG_HEAVYCHECK = false;
#endif

//----------------------------------------------------------------------------

/** Sets the global user abort flag.
    This flag should be set to false at the beginning of each user event,
    e.g. each GUI event or GTP command.
    Lengthy functions should poll the user abort flag with SgUserAbort and
    abort, if necessary; they should not reset the flag themselves.
    It can also be called from a different thread (the abort flag is
    declared volatile).
*/
void SgSetUserAbort(bool aborted);

/** Poll for user abort.
    @see SgSetUserAbort.
*/
bool SgUserAbort();

//----------------------------------------------------------------------------

#endif // SG_SYSTEM_H
