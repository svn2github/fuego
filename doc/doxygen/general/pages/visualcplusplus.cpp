/** @page generalvisualcplusplus Building Fuego with Visual C++

    @section generalvisualcplusplussupport Support for Visual C++

    Recent versions of Fuego should compile with Visual C++. However,
    currently the compilation with Visual C++ is not regularly tested.

    @section generalvisualcplusplusproject How to create a project file

    Here is how to create a Visual C++ project for Fuego. It assumes that
    Visual C++ is installed on your system.

    -# Check out the Fuego code from SVN. On Windows,
    <a href="http://tortoisesvn.tigris.org/">TortoiseSVN</a> is an excellent
    SVN client.
    -# Download and compile the
    <a href="http://www.boost.org/">Boost libraries</a> with Visual C++.
    For example, with Boost version 1.45, download and unpack
    @c boost_1_45_0.7z from
    http://sourceforge.net/projects/boost/files/boost/1.45.0/ and compile it
    with
    @verbatim
    bootstrap
    .\bjam @endverbatim
    This should create static libraries, for example:
    @verbatim
    boost_1_45_0\bin.v2\libs\thread\build\msvc-10.0\release\link-static\threading-multi\libboost_thread-vc100-mt-gd-1_45.lib @endverbatim
    -# Create a new solution named Fuego with Visual C++. The type should be:
    Console Application from existing files. Add all header and cpp files from
    the subdirectories @c gtpengine, @c smartgame, @c go, @c gouct and
    @c fuegomain by dragging and dropping them into the solution explorer
    window (you might want to create subfolders in the solution explorer for
    the different Fuego libraries for better organization).
    -# Add the directories @c gtpengine, @c smartgame, @c go, @c gouct from
    the Fuego source directory to Additional Include Directories in the
    Project properties (right click on the project in the solution explorer
    to get to the Project properties).
    -# Add the subdirectories for all necessary boost libraries to
    Additional Linker Libraries in the project properties. This needs to
    be done differently for Debug and Release configuration.
    -# Copy @c fuego/book/book.dat to the Debug and Release subdirectories
    of the project directory.
    -# You should now be able to compile Fuego and use it in GoGui by
    attaching the executable Fuego.exe in the project build directory.

    @section generalvisualcplusplusmisc Other hints

    -# Enable Insert Spaces in the Tab Settings of the editor in the project
    settings (see @ref generalstyle).
*/

