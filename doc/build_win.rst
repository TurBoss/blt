
HOW TO BUILD BLT ON WINDOWS
===========================

Windows 32-bit and 64-bit have been tested with Tcl/Tk 8.4-8.6.  You'll
need a **cygwin** or **msys** set up to compile.  *I no longer support VC++,
Borland C, etc.*

The configure script determines if you are building for a 32-bit or 64-bit
system from the compiler used.

PREREQUISITES
=============

BLT uses with the autoconf tools to build.  You need either a **cygwin**
or **msys** installed.   Please note that under **cygwin**, the Windows
runtime library is used instead of **cygwin**'s.  

The only other requirement for building BLT is for the Tcl/Tk headers and
include files to installed.  You can build Tcl/Tk yourself, or install them
from a distributed package. BLT does not require the **Tcl/Tk** C source
files.

  **Tcl/Tk 8.4-8.6**		
    The headers and include files must be installed.  
	
The following libraries and include files are optional.  They are used in
specific BLT sub-packages.

  **expat**
    for blt::datatable and blt::tree XML parsers.
  **jpeg**			
    for picture image to read jpeg files.		  
  **png**		
    for picture image to read png files.		  
  **tiff**
    for picture image to read tiff files.		  
  **freetype**
    for drawing text on picture images.
  **libssh2**
    for sftp package.		  
  **sqlite**
    for blt::datatable.
  **libpostgres**
    for blt::datatable.
  **libmysqlclient**
    for blt::datatable.

INSTALLATION LOCATIONS
======================

The location where BLT will be installed is determined by the *prefix*
and *exec-prefix* values.  

   Include files
     *prefix*/include 
   Libraries
      *exec-prefix*/lib
   bltwish, bltsh
     *exec-prefix*/bin
   Scripts and modules
     *exec-prefix*/lib/blt3.0

The *prefix*/include and *exec-prefix*/lib directories are also used to
automatically search for include files and libraries for packages.  

The default *exec-prefix* and *prefix* directory is "/usr/local".  You can
specify *prefix* and *exec-prefix* with the **--prefix** and
**--exec-prefix** switches respectively.

The directories *prefix* and *exec-prefix* must already exist before
installing BLT.

CHOOSING TCL/TK
===============

BLT needs the location of where Tcl/Tk are installed. By default this is
determined from where the **tclsh** or **wish** program is installed.  This
means that **tclsh** and **wish** must be found in the **PATH** environment
variable.  Otherwise, the known locations (*prefix*/include, /usr/include,
/usr/local/include, *exec-prefix*/lib, /usr/lib, /usr/local/lib) are
searched.

You can specify where Tcl and Tk are installed with the **--with-tcl** and
**--with-tk** switches.  The **--with-tcl** switch specifies the directory
containing the "tclConfig.sh" file.  The **--with-tk** switch specifies the
directory containing the "tkConfig.sh" file.  The tclConfig.sh and
tkConfig.sh are shell scripts describe where the Tcl/Tk include files
and libraries are installed.

You can manually specify the directories where the Tcl and Tk header files
and libraries are installed respectively with the **--with-tclincdir**,
**--with-tcllibdir**, **--with-tkincdir**, and **--with-tklibdir**
switches.  These switches override values found in the tclConfig.sh or
tkConfig.sh files.

BUILD STEPS
===========

1. Unpack the BLT tar file.

 ::

       tar -xvf blt.tgz 

 This will create a "blt" directory.

2. Create a build directory.  You can build BLT either inside the source
   directory or in in a separate directory.

  ::

       mkdir build
       cd build

3. Note For **cygwin**:  To compile a native windows version with the **cygwin**
   compiler you'll need to specify the compiler with the CC environment
   variable.  

   ::

	CC=i686-x64-ming32-gcc
	export CC

   Both Tcl and Tk must also be a native windows version (non-cygwin).

   You don't have do anything to compile a cygwin version.

3. Run "configure" to set what packages an options you want.

   ::

       ../configure --prefix=$HOME/blt \
                    --exec-prefix=$HOME/blt 

   Add --enable-stubs to build stub-ed versions.  Configure will look 
   for Tcl/Tk in the usual locations (/usr, /usr/local) and install it
   in a "blt" directory off of your $HOME directory.

   Example:
 
   ::

       ../configure --prefix=C:/tcltk \
          --exec-prefix=C:/tcltk
          --with-expatlibdir=$(libdir) \
          --with-pnglibdir=$(libdir) \
          --with-jpeglibdir=$(libdir) \
          --with-tifflibdir=$(libdir) \
          --with-freetype2libdir=$(libdir) \
          --with-libssh2libdir=$(libdir) \
          --with-libssh2incdir=$(incdir) \
          --with-openssllibdir=$(SSLDIR)/lib \
          --enable-shared \
          $(common_flags)


CONFIGURE OPTIONS 
=================

 **--enable-shared**
   Created shared libraries.  This is enabled by default.
 **--enable-stubs**
   Build stubbed version of BLT libraries.  
 **--enable-symbols**
   Compile with debugging symbols.  
 **--enable-xshm**		    
   The is option in not used for Windows.
 **--with-blt=**\ *dir*             
   Install BLT scripts in *dir*. The default is to install BLT scripts in
   *exec-prefix*/lib/blt3.0.
 **--with-expatincdir=**\ *dir*     
   Find expat headers in *dir*.  Expat is used for XML parsing in the BLT
   **datatable** and **tree** objects.  If *dir* is "yes", the expat headers
   are searched in *prefix*/include, /usr/include and /usr/local/include.
   The default is "yes".
 **--with-expatlibdir=**\ *dir*     
   Find expat libraries in *dir*. Expat is used for XML parsing in the BLT
   **datatable** and **tree** objects.  If *dir* is "yes", the expat libraries
   are searched in *exec-prefix*/lib, /usr/lib, and /usr/local/lib.  The
   default is "yes".
 **--with-freetype2incdir=**\ *dir* 
   Find freetype2 headers in *dir*.  Freetype is used for rendering
   antialiased and rotated fonts and drawing text in **picture** images.
   If *dir* is "yes", the include files are searched in *prefix*/include,
   /usr/include and /usr/local/include.  The default is "yes".
 **--with-freetype2libdir=**\ *dir* 
   Find freetype2 libraries in *dir*.  Freetype is used for rendering
   antialiased and rotated fonts and drawing text in **picture** images.
   If *dir* is "yes", the freetype libraries are searched in
   *exec-prefix*/lib, /usr/lib, and /usr/local/lib.  The default is "yes".
 **--with-gnu-ld**                  
   Use GNU linker.
 **--with-jpegincdir=**\ *dir*      
   Find JPEG headers in *dir*.  Enables reading and writing of JPEG image
   files in the BLT **picture** image.  If *dir* is "yes", the
   include files are searched in *prefix*/include, /usr/include and
   /usr/local/include.  The default is "yes".
 **--with-jpeglibdir=**\ *dir*      
   Find JPEG libraries in *dir*. Enables reading and writing of JPEG image
   files in the BLT **picture** image. If *dir* is "yes", the JPEG libraries
   are searched in *exec-prefix*/lib, /usr/lib and /usr/local/lib.  The
   default is "yes".
 **--with-libssh2incdir=**\ *dir*   
   Find libssh2 headers in *dir*. Enables **sftp** object that lets you
   transfer files and directories. If *dir* is "yes", the include files are
   searched in *prefix*/include, /usr/include and /usr/local/include.  The
   default is "yes".
 **--with-libssh2libdir=**\ *dir*   
   Find libssh2 libraries in *dir*. Enables **sftp** object that lets you
   transfer files and directories. If *dir* is "yes", the libraries are
   searched in *exec-prefix*/lib, /usr/lib and /usr/local/lib.  The default
   is "yes".
 **--with-mysqlincdir=**\ *dir*     
   Find mysql headers in *dir*.  Enables reading and writing of MySql
   tables in the BLT **datatable** object. If *dir* is "yes", the include
   files are searched in *prefix*/include, /usr/include and
   /usr/local/include.  The default is "yes".
 **--with-mysqllibdir=**\ *dir*     
   Find mysql libraries in *dir*. Enables reading and writing of MySql
   tables in the BLT **datatable** object. If *dir* is "yes", the libraries
   are searched in *exec-prefix*/lib, /usr/lib and /usr/local/lib.  The
   default is "yes".
 **--with-openssllibdir=**\ *dir*   
   Find openssl libraries in *dir*.  Openssl is required for the **sftp**
   object that let you transfer files and directories.  If *dir* is "yes",
   the libraries are searched in *exec-prefix*/lib, /usr/lib and
   /usr/local/lib.  The default is "yes".
 **--with-pngincdir=**\ *dir*       
   Find PNG headers in *dir*. Enables reading and writing of PNG image
   files with the BLT **picture** image. If *dir* is "yes", the include
   files are searched in *prefix*/include, /usr/include and
   /usr/local/include.  The default is "yes".
 **--with-pnglibdir=**\ *dir*       
   Find PNG libraries in *dir*. Enables reading and writing of PNG image
   files with the BLT **picture** image. If *dir* is "yes", the libraries
   are searched in *exec-prefix*/lib, /usr/lib and /usr/local/lib.  The
   default is "yes".
 **--with-pqincdir=**\ *dir*        
   Find Postgres headers in *dir*. Enables reading and writing of Postgres
   tables in the BLT **datatable** object. If *dir* is "yes", the include files
   are searched in *prefix*/include, /usr/include and /usr/local/include.
   The default is "yes".
 **--with-pqlibdir=**\ *dir*        
   Find Postgres libraries in *dir*. Enables reading and writing of Postgres
   tables in the BLT **datatable** object. If *dir* is "yes", the libraries
   are searched in *exec-prefix*/lib, /usr/lib and /usr/local/lib.  The
   default is "yes".
 **--with-sqliteincdir=**\ *dir*    
   Find sqlite headers in *dir*. Enables reading and writing of MySql
   tables in the BLT **datatable** object. If *dir* is "yes", the include files
   are searched in *prefix*/include, /usr/include and /usr/local/include.
   The default is "yes".
 **--with-sqlitelibdir=**\ *dir*    
   Find sqlite libraries in *dir*. Enables reading and writing of MySql
   tables in the BLT **datatable** object. If *dir* is "yes", the libraries
   are searched in *exec-prefix*/lib, /usr/lib and /usr/local/lib.  The
   default is "yes".
 **--with-tcl=**\ *dir*             
   Find tclConfig.sh in *dir*.  Loads the configuration information from
   the tclConfig.sh file in *dir*.  This specifies the location of the TCL
   header files and libraries. If *dir* is "yes", the tclConfig.sh file is
   searched in *exec-prefix*/lib, /usr/lib and /usr/local/lib.  The default
   is "yes".
 **--with-tclincdir=**\ *dir*       
   Find Tcl includes in *dir*. Specifies the directory containing the TCL
   header files. This overrides the setting found with the **--with-tcl**
   switch.
 **--with-tcllibdir=**\ *dir*       
   Find Tcl libraries in *dir*. Specifies the directory containing the TCL
   libraries. This overrides the setting found with the **--with-tcl**
   switch.
 **--with-tiffincdir=**\ *dir*      
   Find TIFF headers in *dir*. Enables reading and writing of TIFF image
   files with the BLT **picture** image. If *dir* is "yes", the include
   files are searched in *prefix*/include, /usr/include and
   /usr/local/include.  The default is "yes".
 **--with-tifflibdir=**\ *dir*      
   Find TIFF libraries in *dir*. Enables reading and writing of TIFF image
   files with the BLT **picture** image. If *dir* is "yes", the libraries
   are searched in *exec-prefix*/lib, /usr/lib and /usr/local/lib.  The
   default is "yes".
 **--with-tk=**\ *dir*              
   Find tkConfig.sh in *dir*.  Loads the configuration information from
   the tkConfig.sh file in *dir*.  This specifies the location of the Tk
   header files and libraries. If *dir* is "yes", the tkConfig.sh file is
   searched in *exec-prefix*/lib, /usr/lib and /usr/local/lib.  The default
   is "yes".
 **--with-tkincdir=**\ *dir*        
   Find Tk includes in *dir*.  Specifies the directory containing the
   Tk header files. This overrides the setting found with the **--with-tk**
   switch.  
 **--with-tklibdir=**\ *dir*        
   Find Tk libraries in *dir*. Specifies the directory containing the Tk
   libraries. This overrides the setting found with the **--with-tk**
   switch.
 **--with-xftincdir=**\ *dir*       
   Find Xft headers in *dir*. Enables antialiased and rotated font
   rendering. If *dir* is "yes", the include files are searched in
   *prefix*/include, /usr/include and /usr/local/include.  The default is
   "yes".
 **--with-xftlibdir=**\ *dir*       
   The is option in not used for Windows.
 **--with-xpmincdir=**\ *dir*       
   Find XPM headers in *dir*. Enables reading and writing of XPM image
   files with the BLT **picture** image. If *dir* is "yes", the include
   files are searched in *prefix*/include, /usr/include and
   /usr/local/include.  The default is "yes".
 **--with-xpmlibdir=**\ \ *dir*     
   Find XPM libraries in *dir*.  Enables reading and writing of XPM image
   files with the BLT **picture** image.  If *dir* is "yes", the libraries
   are searched in *exec-prefix*/lib, /usr/lib and /usr/local/lib.  The
   default is "yes".
 **--with-xrandrincdir=**\ *dir*    
   The is option in not used for Windows.
 **--with-xrandrlibdir=**\ *dir*    
   The is option in not used for Windows.
 **--with-zlibdir=**\ *dir*         
   Find zlib libraries in *dir*. Zlib is used with expat for XML parsing in
   the BLT **datatable** and **tree** objects. If *dir* is "yes", the
   libraries are searched in *exec-prefix*/lib, /usr/lib and
   /usr/local/lib.  The default is "yes".

NOTES
=====

gcc mingw32 or mingw64 can create Windows executables and DLLs.

1.  If you are building Tk 8.5 with mingw32/64, you need to fix the 
    source code in win/winMain.c to add __MINGW32__ to the __CYGWIN__ 
    defines.

    sed -i 's/defined(__CYGWIN__)/defined(__CYGWIN__) || defined(MINGW32)' 
       win/winMain.c

2.  The bltwish demo is a statically built executable. It doesn't
    work with --enable-stubs.
