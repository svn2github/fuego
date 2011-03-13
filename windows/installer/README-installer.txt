This directory contains files for creating a Windows installer for Fuego
using NSIS (http://nsis.sourceforge.net)

1. Compile a Windows version of Fuego with Visual Studio or MinGW
   following the documentation in the developer documentation of Fuego
   and copy the resulting file fuego.exe in this directory.
2. Copy the files book/book.dat and doc/manual/index.html from the same Fuego
   version in this directory.
3. Copy the file gogui/lib/gogui.jar from http://gogui.sourceforge.net
   into this directory.
4. Install NSIS and run the command "makensis install.nsis" in this
   directory. This will create an installer named Install.exe.
