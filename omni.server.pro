TEMPLATE = app
CONFIG = console

!exists($$[COMMON_PATH]/omni.common.pro) {
   error(unable to find omni.common.  Please set the path using: qmake -set COMMON_PATH [path to omni.common])
}

## start of section to be rewritten using Perl
HEADERS +=  \
	src/chunks/rawChunkSlicer.hpp \
	src/datalayer/IDataVolume.hpp \
	src/datalayer/dataWrapper.h \
	src/datalayer/fs/IOnDiskFile.h \
	src/datalayer/fs/file.h \
	src/datalayer/fs/memMappedFile.hpp \
	src/mesh/io/chunk/memMappedAllocFile.hpp \
	src/mesh/io/chunk/meshChunkDataReader.hpp \
	src/mesh/io/chunk/meshChunkTypes.h \
	src/mesh/io/dataForMeshLoad.hpp \
	src/mesh/io/meshReader.hpp \
	src/pipeline/bitmask.hpp \
	src/pipeline/encode.hpp \
	src/pipeline/filter.hpp \
	src/pipeline/getSegIds.hpp \
	src/pipeline/getTileData.hpp \
	src/pipeline/jpeg.h \
	src/pipeline/png.hpp \
	src/pipeline/sliceTile.hpp \
	src/pipeline/stage.hpp \
	src/pipeline/utility.hpp \
	src/pointsInCircle.hpp \
	src/segment/segmentTypes.h \
	src/serverHandler.hpp \
	src/serverHandlerNew.hpp \
	src/tiles/tileFilters.hpp

SOURCES +=  \
	src/datalayer/fs/file.cpp \
	src/main.cpp \
	src/pipeline/jpeg.cpp


## end of section to be rewritten using Perl

SOURCES += include/libb64/src/cencode.c

INCLUDEPATH = src include lib
INCLUDEPATH += external/zi_lib
INCLUDEPATH += include/yaml-cpp/include
INCLUDEPATH += include/libb64/include
INCLUDEPATH += $$[COMMON_PATH]/lib/include
LIBS += $$[COMMON_PATH]/lib/bin/libomni.common.a

OBJECTS_DIR = build
MOC_DIR = build
RCC_DIR = build
DESTDIR = bin
TARGET = omni.server

#### for static build
#CONFIG += qt warn_on static
#QMAKE_LFLAGS += -static

####
# if gcc > 4.1, make non-void functions not returning something generate an error
# from http://stackoverflow.com/questions/801279/finding-compiler-vendor-version-using-qmake
linux-g++ {
    system( g++ --version | grep -e "4.[2-9]" ) {
        message( g++ version 4.[2-9] found )
        CONFIG += g++4new
    }
    else {
        CONFIG += g++old
        message( old g++ found )
    }
}
# http://gcc.gnu.org/bugzilla/show_bug.cgi?id=43943
g++4new: QMAKE_CXXFLAGS += -Werror=return-type

####
# debug/release
# if mac or ./omni/release files exists, force release mode; else debug
CONFIG -= debug
CONFIG -= release
mac {
    CONFIG += release
} else {
    exists(release) {
       CONFIG += release
    }else {
       CONFIG += debug
    }
}
CONFIG(release, debug|release) {
    message ( in release mode; adding NDEBUG and no-strict-aliasing )
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS += -DNDEBUG  -fno-strict-aliasing -O3 -fno-omit-frame-pointer -g
    QMAKE_LFLAGS   += -DNDEBUG
}

# GCC Parallel Mode support
linux-g++ {
    system( g++ --version | grep -e "4.[3-9]" ) {
        message( assuming g++ Parallel Mode supported )
        QMAKE_CXXFLAGS += -DZI_USE_OPENMP -fopenmp
        QMAKE_LFLAGS   += -DZI_USE_OPENMP -fopenmp
    }
}

# so QT wont define any non-all-caps keywords
CONFIG += no_keywords

#### for profiling
#QMAKE_CXXFLAGS += -pg
#QMAKE_LFLAGS   += -pg

!exists(gprofile) {
  CONFIG(debug, debug|release) {
      message ( in debug mode ; adding gstabs+ )
      QMAKE_CXXFLAGS += -gstabs+
      QMAKE_LFLAGS   += -gstabs+
  }
} else {
  QMAKE_LFLAGS   += -lprofiler
  QMAKE_CXXFLAGS += -fno-omit-frame-pointer
}

#### Boost
exists(external/libs/Boost) {
     INCLUDEPATH += external/libs/Boost/include

     LIBS += external/libs/Boost/lib/libboost_filesystem.a
     LIBS += external/libs/Boost/lib/libboost_iostreams.a
     LIBS += external/libs/Boost/lib/libboost_system.a
     LIBS += external/libs/Boost/lib/libboost_thread.a
     LIBS += external/libs/Boost/lib/libboost_regex.a

     QMAKE_CXXFLAGS += -DBOOST_MULTI_INDEX_DISABLE_SERIALIZATION
     QMAKE_CXXFLAGS += -DBOOST_SPIRIT_THREADSAFE
     QMAKE_CXXFLAGS += -DBOOST_SYSTEM_NO_DEPRECATED
     QMAKE_CXXFLAGS += -DBOOST_FILESYSTEM_VERSION=3
     QMAKE_CXXFLAGS += -DBOOST_FILESYSTEM_NO_DEPRECATED

#BOOST_DISABLE_ASSERTS

}else {
    error(please run 'bootstrap.pl 2' to install Boost)
}

#### thrift
exists(external/libs/thrift) {
    INCLUDEPATH += external/libs/thrift/include/thrift/

    LIBS += external/libs/thrift/lib/libthrift.a
} else {
    error(please run 'bootstrap.pl 3' to install thrift)
}

#### libjpeg
exists(external/libs/libjpeg) {
    INCLUDEPATH += external/libs/libjpeg/include

    LIBS += external/libs/libjpeg/lib/libturbojpeg.a
} else {
    error(please run 'bootstrap.pl 4' to install libjpeg)
}

#### libpng
exists(external/libs/libpng) {
    INCLUDEPATH += external/libs/libpng/include

    LIBS += external/libs/libpng/lib/libpng.a
} else {
    error(please run 'bootstrap.pl 5' to install libpng)
}

#### zlib
exists(external/libs/zlib) {
    INCLUDEPATH += external/libs/zlib/include

    LIBS += external/libs/zlib/lib/libz.a
} else {
    error(please run 'bootstrap.pl 6' to install zlib)
}
