
How to build BLT on Unix
========================

I've tested BLT on 32-bit and 64-bit Unix systems with Tcl/Tk 8.4-8.6.
It builds with both **clang** and **gcc**.

The configure script determines if you are building for a 32-bit or 64-bit
system from the compiler used.

Prerequisites
=============

The only requirement for building BLT is for the Tcl/Tk headers and include
files to installed.  You can build Tcl/Tk yourself, or install them from a
distributed package. BLT does not require the **Tcl/Tk** C source files.

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

Choosing Tcl/Tk
===============

BLT needs the location of where Tcl/Tk are installed. By default this is
determined from where the **wish** program is installed.  This means that
**wish** must be in the **PATH** environment variable.  Otherwise, the 
known locations (/usr, /usr/local) are searched.

You can specify where Tcl and Tk are installed with the **--with-tcl** and
**--with-tk** switches.  The **--with-tcl** switch specifies the directory
containing the "tclConfig.sh" file.  The **--with-tk** switch specifies the
directory containing the "tkConfig.sh" file.  The tclConfig.sh and
tkConfig.sh are shell scripts that describe where the Tcl/Tk include files
and libraries are installed.

You can manually specify the directories where the Tcl and Tk header files
and libraries are installed respectively with the **--with-tclincdir**,
**--with-tcllibdir**, **--with-tkincdir**, and **--with-tklibdir**
switches.  You can also override values found in the tclConfig.sh or
tkConfig.sh with these switches.

Installation locations
======================

The location where BLT will be installed is determined by the *prefix*
and *exec-prefix* values.  

   Include files:	*prefix*/include 
   Libraries:		*exec-prefix*/lib
   bltwish, bltsh:	*exec-prefix*/bin
   Scripts and modules	*exec-prefix*/lib/blt3.0

By default, *exec-prefix* and *prefix* are the same as what was used for
the Tcl installation. The **TCL_EXEC_PREFIX** value is used from the
tclConfig.sh file. The tclConfig.sh file is either found in a known
location or specified by **--with-tcl**.  If no tclConfig.sh is found,
*prefix* and *exec-prefix* are "/usr/local".

You can specify *prefix* and *exec-prefix* with the **--prefix** and
**--exec-prefix** switches respectively. 

The directories *prefix* and *exec-prefix* point to must already exist
before installing BLT.

Build Steps
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

3. Run "configure". Specify what packages and options you want.

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


Configure Options 
=================

 ================================== ========================================
 Configure Option                   Description                       
 ---------------------------------- ----------------------------------------
 **--enable-shared**	            Created shared libraries.  
 **--enable-stubs**		    Build stubbed version of BLT libraries.
 **--enable-symbols**		    Compile with debugging symbols.
 **--enable-xshm**		    Use X Shared Memory extension   
 **--with-blt=**\ *dir*             Install BLT scripts in *dir*       
 **--with-expatincdir=**\ *dir*     Find expat headers in *dir*  
 **--with-expatlibdir=**\ *dir*     Find expat libraries in *dir*  
 **--with-freetype2incdir=**\ *dir* Find freetype2 headers in *dir*  
 **--with-freetype2libdir=**\ *dir* Find freetype2 libraries in *dir*  
 **--with-gnu-ld**                  Use GNU linker.
 **--with-jpegincdir=**\ *dir*      Find JPEG headers in *dir*          
 **--with-jpeglibdir=**\ *dir*      Find JPEG libraries in *dir*        
 **--with-libssh2incdir=**\ *dir*   Find libssh2 headers in *dir*  
 **--with-libssh2libdir=**\ *dir*   Find libssh2 libraries in *dir*  
 **--with-mysqlincdir=**\ *dir*     Find mysql headers in *dir*  
 **--with-mysqllibdir=**\ *dir*     Find mysql libraries in *dir*  
 **--with-openssllibdir=**\ *dir*   Find openssl libraries in *dir*  
 **--with-pngincdir=**\ *dir*       Find PNG headers in *dir*           
 **--with-pnglibdir=**\ *dir*       Find PNG libraries in *dir*         
 **--with-pqincdir=**\ *dir*        Find Postgres headers in *dir*  
 **--with-pqlibdir=**\ *dir*        Find Postgres libraries in *dir*  
 **--with-sqliteincdir=**\ *dir*    Find sqlite headers in *dir*  
 **--with-sqlitelibdir=**\ *dir*    Find sqlite libraries in *dir*  
 **--with-tcl=**\ *dir*             Find tclConfig.sh in *dir*
 **--with-tclincdir=**\ *dir*       Find Tcl includes in *dir*         
 **--with-tcllibdir=**\ *dir*       Find Tcl libraries in *dir*         
 **--with-tiffincdir=**\ *dir*      Find TIFF headers in *dir*          
 **--with-tifflibdir=**\ *dir*      Find TIFF libraries in *dir*        
 **--with-tk=**\ *dir*              Find tkConfig.sh in *dir*
 **--with-tkincdir=**\ *dir*        Find Tk includes in *dir*           
 **--with-tklibdir=**\ *dir*        Find Tk libraries in *dir*          
 **--with-xftincdir=**\ *dir*       Find Xft headers in *dir*  
 **--with-xftlibdir=**\ *dir*       Find Xft libraries in *dir*  
 **--with-xpmincdir=**\ *dir*       Find XPM headers in *dir*           
 **--with-xpmlibdir=**\ \ *dir*     find XPM libraries in *dir*  
 **--with-xrandrincdir=**\ *dir*    Find Xrandr headers in *dir*
 **--with-xrandrlibdir=**\ *dir*    Find Xrandr libraries in *dir*  
 **--with-zlibdir=**\ *dir*         Find zlib libraries in *dir*        
 ================================== ========================================

1.  If you are building Tk 8.5 with mingw32/64, you need to fix the 
    source code in win/winMain.c to add __MINGW32__ to the __CYGWIN__ 
    defines.

    sed -i 's/defined(__CYGWIN__)/defined(__CYGWIN__) || defined(MINGW32)' 
       win/winMain.c

2.  The bltwish demo is a statically built executable. It doesn't
    work with --enable-stubs.
