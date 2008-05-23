/** @page generalgmake Building with GNU make

    Currently, there exists a build system using GCC/GNU make that is tested
    on recent Linux distributions and Mac OS X.

    @section generalrequirements Requirements

    - GCC (currently version 3.2 or higher should work)
    - GNU make
    - Boost libraries (http://www.boost.org)

    @section generalbuild Building

    Fuego is split into several modules, which can be libraries
    (static libraries) or applications (executables).

    There is a global Makefile in the <tt>build/gmake</tt> subfolder
    for building all projects in the right order.

    - <tt>make debug</tt>
      Builds the debug version
    - <tt>make release</tt>
      Builds the release version (with optimizations and without assertions)
    - <tt>make</tt>
      Builds both versions

    Use the option <tt>-s</tt> for make to get less verbose output
    during compilation.

    The resulting libraries and executables are in
    <tt>build/gmake/build/debug</tt> and <tt>build/gmake/build/release</tt>.<br>
    It is possible to build only one project by doing a <tt>make</tt>
    in the module's subfolder.<br>
    This will not check dependencies between modules.

    @section generalprojects Creating Projects

    If new projects should be created that are not going to be part
    of Fuego, it is recommended to create a directory parallel
    to <tt>fuego</tt>, create similar Makefiles in it
    and include the Fuego headers with relative pathnames.
*/

