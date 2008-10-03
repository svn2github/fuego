/** @page generalautotools Building Fuego using GNU Autotools

@section generalautotoolsdistro Building a distribution (released version)

@verbatim
./configure
make
@endverbatim

@section generalautotoolssvn Building a development version checked out from SVN

@verbatim
aclocal
autoheader
automake --add-missing
autoreconf
automake --add-missing
@endverbatim

The above commands need to be run only initially. Then the compilation works
as in the previous section.

After adding or removing files or doing other changes to
<tt>configure.in</tt> or a <tt>Makefile.am</tt>, you need to run
<tt>autoreconf</tt> again bevor doing a make.
A better way is to configure your makefiles with
<tt>./configure --enable-maintainer-mode</tt>. Then a make will automatically
check, if <tt>configure.in</tt> or a <tt>Makefile.am</tt> have changed and
recreate the makefiles bevor the compilation if necessary.

Other interesting options to configure are @c --enable-assert for enabling
assertions or @c --enable-optimize=no for switching off optimizations.
<tt>./configure --help</tt> returns a full list of options.

@section generalautotoolsinstall Installing Fuego

@verbatim
sudo make install
@endverbatim

*/

