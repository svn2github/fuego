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
    of MinGW. The version used were MinGW GCC version 4.5.0 and Boost 1.45.0.
    
    -# Check out the Fuego code from SVN or download a distribution of Fuego.
    On Windows, <a href="http://tortoisesvn.tigris.org/">TortoiseSVN</a> is
    an excellent SVN client. 
    -# Install MinGW and MSYS using the MinGW installer.
    -# Download the source for the <a href="http://www.boost.org/">Boost
    libraries</a> and a pre-compiled version of BJam (e.g. 
    boost-jam-3.1.18-1-ntx86.zip). Unpack the files and copy bjam.exe in the
    Boost source directory.
    -# Compile Boost with MinGW in the MSYS shell with the following command
    (this compiles only the libraries used by Fuego):
    @verbatim
    ./bjam.exe --toolset=gcc --layout=tagged --with-thread \
      --with-program_options --with-filesystem --with-system \
      --with-date_time --prefix=/usr install @endverbatim
    This should create static libraries, for example:
    @verbatim
    /usr/lib/libboost_thread-mt.a @endverbatim
    -# Compile Fuego in the MSYS shell. Theoretically, there is support
    for running the configure scripts and using make in MSYS, but currently
    there are problems with the boost detection scripts with MinGW, so we
    simply invoke the compiler in a single command:
    @verbatim
    cd fuego
    mkdir mingw
    cd mingw
    g++ -o Fuego -DNDEBUG -O3 -ffast-math -DBOOST_THREAD_USE_LIB \
      -D_WIN32_WINNT=0x0500 -static-libgcc -static-libstdc++ -I/usr/include \
      -L/usr/lib -I../gtpengine -I../smartgame -I../go -I../gouct \
      ../gtpengine/*.cpp ../smartgame/*.cpp ../go/*.cpp ../gouct/*.cpp \
      ../fuegomain/*.cpp -lboost_thread-mt -lboost_program_options-mt \
      -lboost_date_time-mt -lboost_filesystem-mt -lboost_system-mt @endverbatim
    The macro BOOST_THREAD_USE_LIB is a workaround for a compilation problem
    with Boost 1.45.0 that may not be necessary in the future.
    This should create an executable named Fuego.exe in the current directory.
    -# Copy the file fuego/book/book.dat into the directory of Fuego.exe
*/

