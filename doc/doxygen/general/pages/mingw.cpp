/** @page generalmingw Building Fuego with MinGW

    @section generalmingwsupport Support for MinGW

    Recent versions of Fuego can be compiled with
    <a href="http://www.mingw.org">MinGW</a> on Windows.
    Currently, using MinGW is the best option for building a high-performance
    Windows version of Fuego, because the version compiled with Visual C++
    is about 15 percent slower.
    
    Using <a href="http://www.cygwin.com">Cygwin</a> instead of MinGW is
    another option, but there are currently bugs in the Boost libraries if
    built with Cygwin that make pondering and the automatic detection of
    number of cores not work in the Cygwin version of Fuego (last tested
    with Cygwin GCC version 3.4.4).

    @section generalmingwcompile How to compile Fuego with MinGW

    Here are the necessary steps to compile Fuego with MinGW. There are some
    workarounds for problems that may not be necessary in future versions
    of MinGW. The version used were MinGW GCC version 4.8.1 and Boost 1.57.0.
    
    -# Check out the Fuego code from SVN or download a distribution of Fuego.
       On Windows, <a href="http://tortoisesvn.tigris.org/">TortoiseSVN</a> is
       an excellent SVN client. 
    -# Determine whether you want to compile Fuego for a 32-bit or 64-bit system.
       The Fuego source code can be compiled for either version, but will require
       either the 32-bit or 64-bit versions of the Boost Libraries and MinGW
       depending on which one you choose.
    -# Install MinGW and MSYS
       - For 64-bit you will need
         <a href="http://mingw-w64.sourceforge.net/">MinGW-w64</a>, but this has
         not been tested.
       - For 32-bit, follow the instructions in MinGW's
         <a href="http://mingw.org/wiki/Getting_Started">Getting Started</a>
         guide.
       - In the GUI installer make sure to select the @c mingw32-autotools
         package in the "MINGW" section. This is required so that @c autoreconf
         is available later on during the Fuego configure process.
       - After the GUI installer has finished installing MinGW, open an MSYS
         shell and type the following command to manually install the POSIX
         threads library. Without this step the Fuego configure process will
         fail.
    @verbatim
    mingw-get install pthreads
    @endverbatim

    -# Install and compile Boost
       - Download and unpack the source for the
         <a href="http://www.boost.org/">Boost libraries</a>. For example, with
         Boost version 1.57, download and unpack @c boost_1_57_0.7z from
         http://sourceforge.net/projects/boost/files/boost/1.57.0/.
       - Open a CMD shell (<em>not</em> an MSYS shell!), then type the following
         commands. This will compile Boost's build tool BJam.
    @verbatim
    set PATH=C:\MinGW\bin;%PATH%
    cd \path\to\boost
    bootstrap.bat mingw
    @endverbatim
    
       - Open an MSYS shell, then type the following commands. This will compile the
         Boost libraries (not all, only those used by Fuego).
    @verbatim
    b2 --toolset=gcc --layout=tagged --with-thread \
       --with-program_options --with-filesystem --with-system \
       --with-date_time --with-test --prefix=/usr install
    @endverbatim
    
       - Be sure to use the @c b2 command line option "--layout=tagged" so that
         the library names that are generated look like this
    @verbatim
    /usr/lib/libboost_thread-mt.a
    @endverbatim
         instead of this
    @verbatim
    /usr/lib/libboost_thread-mgw48-mt.a
    @endverbatim
         Also, the header files are installed in
    @verbatim
    /usr/include/boost
    @endverbatim
         instead of
    @verbatim
    /usr/include/boost-1_57
    @endverbatim
    
    -# Compile Fuego
       - Note that if you are compiling as a 32-bit program you may want to
         set the @c /LARGEADDRESSAWARE flag to YES. This allows up to ~3.5 GB
         of memory usage (as opposed to 2 GB) and can be done by adding
    @verbatim
    LDFLAGS="-Wl,--large-address-aware"
    @endverbatim
         between @c env and @c CXXFLAGS below.
       - Open an MSYS shell, then type the following commands. This should
         create the executable named @c fuegomain/fuego.exe.
    @verbatim
    cd \path\to\fuego
    autoreconf -i
    mkdir mingw
    cd mingw
    env CXXFLAGS="-O3 -ffast-math -static-libgcc -static-libstdc++" ../configure
    make
    @endverbatim
    -# Copy the file @c fuego/book/book.dat into the directory of @c fuego.exe
*/
