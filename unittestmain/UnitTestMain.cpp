//----------------------------------------------------------------------------
/** @file UnitTestMain.cpp
    Main function for running unit tests from all modules in project Fuego. */
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include "GoInit.h"
#include "SgInit.h"

//----------------------------------------------------------------------------

namespace {

void Init()
{
    SgInit();
    GoInit();
}

void Fini()
{
    GoFini();
    SgFini();
}

} // namespace

//----------------------------------------------------------------------------
// Handling of unit testing framework initialization is messy and not
// documented in the Boost 1.34 documentation. See also:
// http://lists.boost.org/Archives/boost/2006/11/112946.php

// Also see:
// http://www.boost.org/doc/libs/1_56_0/libs/test/doc/html/utf/user-guide/initialization.html
// http://www.boost.org/doc/libs/1_56_0/libs/test/doc/html/utf/compilation.html#utf.flag.main
// http://www.boost.org/doc/libs/1_60_0/libs/test/doc/html/boost_test/adv_scenarios/test_module_init_overview.html

#include <cstdlib>
#define BOOST_TEST_DYN_LINK // Must be defined before including unit_test.hpp

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
    #include <boost/test/unit_test.hpp>
#pragma clang diagnostic pop


bool init_unit_test()
{
    try
    {
        Init();
    }
    catch (const std::exception& e)
    {
        return false;
    }
    if (std::atexit(Fini) != 0)
        return false;
    return true;
}

int main(int argc, char* argv[])
{
    return boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}

//----------------------------------------------------------------------------
