TEMPLATE = lib
CONFIG = staticlib

## start of section to be rewritten using Perl
HEADERS +=  \
	src/boost.h \
	src/colors.h \
	src/common.h \
	src/container.hpp \
	src/coordinates/chunkCoord.h \
	src/coordinates/coordinates.h \
	src/coordinates/dataCoord.h \
	src/coordinates/globalCoord.h \
	src/coordinates/normCoord.h \
	src/coordinates/screenCoord.h \
	src/coordinates/screenSystem.h \
	src/coordinates/volumeSystem.h \
	src/coordinates/yaml.h \
	src/debug.h \
	src/enums.hpp \
	src/exception.h \
	src/gl.h \
	src/math.hpp \
	src/set.hpp \
	src/std.h \
	src/stoppable.h \
	src/string.hpp \
	src/utility/affinityGraphDataWrapper.hpp \
	src/utility/channelDataWrapper.hpp \
	src/utility/color.hpp \
	src/utility/dataWrapperContainer.hpp \
	src/utility/dataWrappers.h \
	src/utility/filterDataWrapper.hpp \
	src/utility/fuzzyStdObjs.hpp \
	src/utility/image/bits/omImage_traits.hpp \
	src/utility/image/omImage.hpp \
	src/utility/localPrefFiles.h \
	src/utility/localPrefFilesImpl.hpp \
	src/utility/omChunkVoxelWalker.hpp \
	src/utility/omCopyFirstN.hpp \
	src/utility/omDataTime.hpp \
	src/utility/omFileHelpers.h \
	src/utility/omFileLogger.hpp \
	src/utility/omLockedObjects.h \
	src/utility/omLockedPODs.hpp \
	src/utility/omPrimeNumbers.hpp \
	src/utility/omRand.hpp \
	src/utility/omRandColorFile.hpp \
	src/utility/omSharedPtr.hpp \
	src/utility/omSimpleProgress.hpp \
	src/utility/omSmartPtr.hpp \
	src/utility/omStringHelpers.h \
	src/utility/omSystemInformation.h \
	src/utility/omTempFile.hpp \
	src/utility/omTimer.hpp \
	src/utility/omUUID.hpp \
	src/utility/segmentDataWrapper.hpp \
	src/utility/segmentationDataWrapper.hpp \
	src/utility/setUtilities.h \
	src/utility/sortHelpers.h \
	src/utility/yaml/baseTypes.hpp \
	src/utility/yaml/genericManager.hpp \
	src/utility/yaml/mipVolume.hpp \
	src/utility/yaml/yaml.hpp \
	src/zi/matlab/zmex.hpp \
	src/zi/omMutex.h \
	src/zi/omThreads.h \
	src/zi/omUtility.h \
	src/zi/trees/DisjointSets.hpp \
	src/zi/trees/lib/DynaTree.hpp \
	src/zi/trees/lib/DynamicTree.hpp \
	src/zi/trees/lib/FHeap.hpp \
	src/zi/watershed/MemMap.hpp \
	src/zi/watershed/RawQuickieWS.h

SOURCES +=  \
	src/colors.cpp \
	src/common.cpp \
	src/coordinates/chunkCoord.cpp \
	src/coordinates/dataCoord.cpp \
	src/coordinates/globalCoord.cpp \
	src/coordinates/normCoord.cpp \
	src/coordinates/screenCoord.cpp \
	src/coordinates/screenSystem.cpp \
	src/coordinates/volumeSystem.cpp \
	src/coordinates/yaml.cpp \
	src/gl.cpp \
	src/utility/channelDataWrapper.cpp \
	src/utility/omFileHelpers.cpp \
	src/utility/omPrimeNumbers.cpp \
	src/utility/omSystemInformation.cpp \
	src/zi/watershed/RawQuickieWS.cpp


## end of section to be rewritten using Perl

INCLUDEPATH = src include lib
INCLUDEPATH += external/zi_lib
INCLUDEPATH += include/yaml-cpp/include

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

     QMAKE_CXXFLAGS += -DBOOST_MULTI_INDEX_DISABLE_SERIALIZATION
     QMAKE_CXXFLAGS += -DBOOST_SPIRIT_THREADSAFE
     QMAKE_CXXFLAGS += -DBOOST_SYSTEM_NO_DEPRECATED
     QMAKE_CXXFLAGS += -DBOOST_FILESYSTEM_VERSION=3
     QMAKE_CXXFLAGS += -DBOOST_FILESYSTEM_NO_DEPRECATED

#BOOST_DISABLE_ASSERTS

}else {
    error(please run 'bootstrap.pl 1' to install Boost)
}

#### thrift
exists(external/libs/thrift) {
    INCLUDEPATH += external/libs/thrift/include/thrift/

    LIBS += external/libs/thrift/lib/libthrift.a
} else {
    error(please run 'bootstrap.pl 2' to install thrift)
}
