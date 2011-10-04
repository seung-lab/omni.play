TEMPLATE = lib
CONFIG = staticlib

## start of section to be rewritten using Perl
HEADERS +=  \
	lib/include/common/boost.h \
	lib/include/common/colors.h \
	lib/include/common/common.h \
	lib/include/common/container.hpp \
	lib/include/common/coordinates/chunkCoord.h \
	lib/include/common/coordinates/coordinates.h \
	lib/include/common/coordinates/dataCoord.h \
	lib/include/common/coordinates/globalCoord.h \
	lib/include/common/coordinates/normCoord.h \
	lib/include/common/coordinates/screenCoord.h \
	lib/include/common/coordinates/screenSystem.h \
	lib/include/common/coordinates/volumeSystem.h \
	lib/include/common/coordinates/yaml.h \
	lib/include/common/debug.h \
	lib/include/common/enums.hpp \
	lib/include/common/exception.h \
	lib/include/common/gl.h \
	lib/include/common/math.hpp \
	lib/include/common/set.hpp \
	lib/include/common/std.h \
	lib/include/common/stoppable.h \
	lib/include/common/string.hpp \
	lib/include/common/utility/image/bits/image_traits.hpp \
	lib/include/common/utility/image/image.hpp \
	lib/include/common/utility/yaml/baseTypes.hpp \
	lib/include/common/utility/yaml/genericManager.hpp \
	lib/include/common/utility/yaml/mipVolume.hpp \
	lib/include/common/utility/yaml/yaml.hpp \
	lib/include/common/zi/matlab/zmex.hpp \
	lib/include/common/zi/omMutex.h \
	lib/include/common/zi/omThreads.h \
	lib/include/common/zi/omUtility.h \
	lib/include/common/zi/trees/DisjointSets.hpp \
	lib/include/common/zi/trees/lib/DynaTree.hpp \
	lib/include/common/zi/trees/lib/DynamicTree.hpp \
	lib/include/common/zi/trees/lib/FHeap.hpp \
	lib/include/common/zi/watershed/MemMap.hpp \
	lib/include/common/zi/watershed/RawQuickieWS.h \
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
	src/thrift/server.h \
	src/thrift/server_constants.h \
	src/thrift/server_types.h \
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
	src/thrift/server.cpp \
	src/thrift/server_constants.cpp \
	src/thrift/server_types.cpp \
	src/zi/watershed/RawQuickieWS.cpp

server_cpp.target = src/thrift/server.cpp
server_cpp.commands = external/libs/thrift/bin/thrift -r --out src/thrift --gen cpp if/server.thrift
server_cpp.depends = if/server.thrift
QMAKE_EXTRA_TARGETS += server_cpp

server_h.target = src/thrift/server.h
server_h.commands = external/libs/thrift/bin/thrift -r --out src/thrift --gen cpp if/server.thrift
server_h.depends = if/server.thrift
QMAKE_EXTRA_TARGETS += server_h

server_constants_cpp.target = src/thrift/server_constants.cpp
server_constants_cpp.commands = external/libs/thrift/bin/thrift -r --out src/thrift --gen cpp if/server.thrift
server_constants_cpp.depends = if/server.thrift
QMAKE_EXTRA_TARGETS += server_constants_cpp

server_constants_h.target = src/thrift/server_constants.h
server_constants_h.commands = external/libs/thrift/bin/thrift -r --out src/thrift --gen cpp if/server.thrift
server_constants_h.depends = if/server.thrift
QMAKE_EXTRA_TARGETS += server_constants_h

server_types_cpp.target = src/thrift/server_types.cpp
server_types_cpp.commands = external/libs/thrift/bin/thrift -r --out src/thrift --gen cpp if/server.thrift
server_types_cpp.depends = if/server.thrift
QMAKE_EXTRA_TARGETS += server_types_cpp

server_types_h.target = src/thrift/server_types.h
server_types_h.commands = external/libs/thrift/bin/thrift -r --out src/thrift --gen cpp if/server.thrift
server_types_h.depends = if/server.thrift
QMAKE_EXTRA_TARGETS += server_types_h


## end of section to be rewritten using Perl

INCLUDEPATH = src include lib
INCLUDEPATH += external/zi_lib
INCLUDEPATH += include/yaml-cpp/include

OBJECTS_DIR = build
MOC_DIR = build
RCC_DIR = build
DESTDIR = bin
TARGET = omni.common

inst.path = lib/bin
inst.files = bin/*
inst.extra = cd src; find -regex \".*\.h\\\(pp\\\)?\" -print0 | cpio --null -pvd ../lib/include/common/
INSTALLS += inst

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
