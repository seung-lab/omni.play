##                                          ##
##      Omni System Build Instructions      ##
##                                          ##
##  Brett Warne - bwarne@mit.edu - 3/17/09  ##

In order to compile Omni, the following external libraries are needed:
Boost, VTK, Qt, HDF5

Beyond the basic Ubuntu installation, these libraries require the following packages:

build-essential
X11
OpenGL
OSMesa
libtiff
libpng
libjpeg
bzlib
pthreads



1. Create folder named "libs" that you will be placing your libraries in.  The path to this folder will be refered to as <libpath> in the following instructions.




2. VTK
http://www.vtk.org/ 
Download VTK and use the following commands to build the binaries and place them in the lib folder.

ccmake .
Press [c] to configure
Press [enter] on CMAKE_INSTALL_PREFIX to set the install location.
Set CMAKE_INSTALL_PREFIX = <libpath>/VTK
Press [c] to configure
Press [g] to generate the Makefile and exit ccmake.
make
make install

An error of "Cannot create output file." probably means the make was trying to write to the in-source tree.  This can be solved with:
sudo make



2. Boost C++ Library
http://www.boost.org/
Download version 1.37.0 of Boost (or you'll need to figure out bjam) and specify the binary location as your "lib" folder and to only build static libraries.

./configure --prefix=<libpath>/Boost
make
make install

Add the Boost directory to your environment to allow the CMake include scripts find it.  To make this permanent, add the following line to your ~/.bashrc file:

export BOOST_ROOT="<libpath>/Boost"


For more information see the getting started page:
http://www.boost.org/doc/libs/1_37_0/more/getting_started/index.html




3. Qt GUI Library
http://www.qtsoftware.com/downloads
Download the Qt library appropriate for your platform.  Place the source directory inside <libpath> and rename it "Qt" such that the directory is located at <libpath>/Qt.

./configure --prefix=<libpath>/Qt
make

./configure -static -no-framework --prefix=<libpath>/Qt
make sub-src

Add the Qt directory to your environment to allow the CMake include scripts find it.  To make this permanent, add the following line to your ~/.bashrc file:

export QTDIR="<libpath>/Qt"



4. HDF5
http://www.hdfgroup.org/HDF5/
Download version 1.6.8 of the HDF5 library appropriate for your platform.  Specify the install location and enable thread safety.

./configure --enable-threadsafe --with-pthread=/usr/lib --enable-shared=no --prefix=<libpath>/HDF5
make
make install

For thread safety notes refer to:
http://www.hdfgroup.uiuc.edu/papers/features/mthdf/

There is a known problem when building on linux:
Building HDF5 1.6.8, error: call to __open_missing_mode declared with attribute error: open with O_CREAT in second argument needs 3 arguments

A workaround is to edit the ./perform/zip_perf.c file in the source and change line 549 to:

          output = open(filename, O_RDWR | O_CREAT, S_IRWXU);




5. Fontconfig
http://fontconfig.org/wiki/
Download the latest version of Fontconfig.  Specify the install location and disable shared libraries.

./configure -disable-shared --prefix=<libpath>/Fontconfig
make
make install





6. FreeType
http://www.freetype.org/
Download the latest version of FreeType.  Specify the install location and disable shared libraries.

./configure -disable-shared --prefix=<libpath>/FreeType
make
make install



7. libpng
http://www.libpng.org/pub/png/libpng.html
Download and install the latest libpng library.  Specify the install location and disable shared libraries.

./configure -disable-shared --prefix=<libpath>/FreeType
make
make install





8. Update the CMakeLists.txt to reflect these new library locations by changing the string that OM_EXT_LIB_DIR is set to.

# Set library locations
set(OM_EXT_LIB_DIR "<libpath>")



9. Build Omni

cmake .
make

The binaries will be built in the bin folder.  Warnings of missing libraries may be the result of a stale cache file.  Remove with the following command:

rm CMakeCache.txt

And try building again.











IGNORE THE FOLLOWING:
SCRATCH:

./configure --prefix=/Users/bwarne/MIT/School_Work_08-09/MEng/bin/HDF5


cd libs
mkdir -p boost/include
cd boost/include
cp -r .../boost_1_37_0/boost .

For more information see the getting started page:
http://www.boost.org/doc/libs/1_37_0/more/getting_started/index.html

#note: change so it only installs static libraries


4. Freetype
http://www.freetype.org/
./configure
make
make install


5. imlib2
http://docs.enlightenment.org/api/imlib2/html/
./configure
make
make install


5. Update the CMakeLists.txt to reflect these new library locations:
# Set library locations
set(EXT_LIB_DIR "/Users/bwarne/MIT/School_Work_08-09/MEng/libraries")
set(VTK_DIR "${EXT_LIB_DIR}/VTK")
set(BOOST_ROOT "${EXT_LIB_DIR}/boost")

./configure --prefix=/Users/bwarne/MIT/School_Work_08-09/MEng/bin/freetype --enable-shared=no
4. imlib2
export LIBS=" -L/Users/bwarne/MIT/School_Work_08-09/MEng/bin/freetype/lib -lfreetype"
./configure --prefix=/Users/bwarne/MIT/School_Work_08-09/MEng/bin/imlib2 --enable-shared=no
make
make install

