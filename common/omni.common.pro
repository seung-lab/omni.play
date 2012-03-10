TEMPLATE = lib
CONFIG = staticlib

## start of section to be rewritten using Perl
HEADERS +=  \
	lib/include/common/boost.h \
	lib/include/common/colors.h \
	lib/include/common/common.h \
	lib/include/common/container.hpp \
	lib/include/common/debug.h \
	lib/include/common/enums.hpp \
	lib/include/common/exception.h \
	lib/include/common/genericManager.hpp \
	lib/include/common/manageableObject.hpp \
	lib/include/common/math.hpp \
	lib/include/common/set.hpp \
	lib/include/common/std.h \
	lib/include/common/stoppable.h \
	lib/include/common/string.hpp \
	lib/include/coordinates/chunk.h \
	lib/include/coordinates/coordinates.h \
	lib/include/coordinates/data.h \
	lib/include/coordinates/global.h \
	lib/include/coordinates/norm.h \
	lib/include/coordinates/screen.h \
	lib/include/coordinates/screenSystem.h \
	lib/include/coordinates/volumeSystem.h \
	lib/include/coordinates/yaml.h \
	lib/include/threads/taskManager.hpp \
	lib/include/threads/taskManagerContainerDeque.hpp \
	lib/include/threads/taskManagerImpl.hpp \
	lib/include/threads/taskManagerTypes.h \
	lib/include/threads/threadPoolBatched.hpp \
	lib/include/threads/threadPoolByMipLevel.hpp \
	lib/include/threads/threadPoolManager.h \
	lib/include/thrift/FacebookService.h \
	lib/include/thrift/fb303_constants.h \
	lib/include/thrift/fb303_types.h \
	lib/include/thrift/filesystem_constants.h \
	lib/include/thrift/filesystem_types.h \
	lib/include/thrift/server.h \
	lib/include/thrift/server_constants.h \
	lib/include/thrift/server_types.h \
	lib/include/thrift/storage_manager.h \
	lib/include/thrift/storage_server.h \
	lib/include/utility/UUID.hpp \
	lib/include/utility/chunkVoxelWalker.hpp \
	lib/include/utility/copyFirstN.hpp \
	lib/include/utility/dataTime.hpp \
	lib/include/utility/fileHelpers.h \
	lib/include/utility/fileLogger.hpp \
	lib/include/utility/lockedObjects.hpp \
	lib/include/utility/lockedPODs.hpp \
	lib/include/utility/primeNumbers.h \
	lib/include/utility/rand.hpp \
	lib/include/utility/setUtilities.h \
	lib/include/utility/smartPtr.hpp \
	lib/include/utility/systemInformation.h \
	lib/include/utility/tempFile.hpp \
	lib/include/utility/timer.hpp \
	lib/include/utility/yaml/baseTypes.hpp \
	lib/include/utility/yaml/genericManager.hpp \
	lib/include/utility/yaml/yaml.hpp \
	lib/include/zi/mutex.h \
	lib/include/zi/rwlock_pool.hpp \
	lib/include/zi/rwlock_server.hpp \
	lib/include/zi/threads.h \
	lib/include/zi/trees/DisjointSets.hpp \
	lib/include/zi/trees/lib/DynaTree.hpp \
	lib/include/zi/trees/lib/DynamicTree.hpp \
	lib/include/zi/trees/lib/FHeap.hpp \
	lib/include/zi/tri_strip_to_obj.hpp \
	lib/include/zi/utility.h \
	lib/include/zi/watershed/MemMap.hpp \
	lib/include/zi/watershed/RawQuickieWS.h \
	src/common/boost.h \
	src/common/colors.h \
	src/common/common.h \
	src/common/container.hpp \
	src/common/debug.h \
	src/common/enums.hpp \
	src/common/exception.h \
	src/common/genericManager.hpp \
	src/common/manageableObject.hpp \
	src/common/math.hpp \
	src/common/set.hpp \
	src/common/std.h \
	src/common/stoppable.h \
	src/common/string.hpp \
	src/coordinates/chunk.h \
	src/coordinates/coordinates.h \
	src/coordinates/data.h \
	src/coordinates/global.h \
	src/coordinates/norm.h \
	src/coordinates/screen.h \
	src/coordinates/screenSystem.h \
	src/coordinates/volumeSystem.h \
	src/coordinates/yaml.h \
	src/threads/taskManager.hpp \
	src/threads/taskManagerContainerDeque.hpp \
	src/threads/taskManagerImpl.hpp \
	src/threads/taskManagerTypes.h \
	src/threads/threadPoolBatched.hpp \
	src/threads/threadPoolByMipLevel.hpp \
	src/threads/threadPoolManager.h \
	src/thrift/FacebookService.h \
	src/thrift/fb303_constants.h \
	src/thrift/fb303_types.h \
	src/thrift/filesystem_constants.h \
	src/thrift/filesystem_types.h \
	src/thrift/server.h \
	src/thrift/server_constants.h \
	src/thrift/server_types.h \
	src/thrift/storage_manager.h \
	src/thrift/storage_server.h \
	src/utility/UUID.hpp \
	src/utility/chunkVoxelWalker.hpp \
	src/utility/copyFirstN.hpp \
	src/utility/dataTime.hpp \
	src/utility/fileHelpers.h \
	src/utility/fileLogger.hpp \
	src/utility/lockedObjects.hpp \
	src/utility/lockedPODs.hpp \
	src/utility/primeNumbers.h \
	src/utility/rand.hpp \
	src/utility/setUtilities.h \
	src/utility/smartPtr.hpp \
	src/utility/systemInformation.h \
	src/utility/tempFile.hpp \
	src/utility/timer.hpp \
	src/utility/yaml/baseTypes.hpp \
	src/utility/yaml/genericManager.hpp \
	src/utility/yaml/yaml.hpp \
	src/zi/mutex.h \
	src/zi/rwlock_pool.hpp \
	src/zi/rwlock_server.hpp \
	src/zi/threads.h \
	src/zi/trees/DisjointSets.hpp \
	src/zi/trees/lib/DynaTree.hpp \
	src/zi/trees/lib/DynamicTree.hpp \
	src/zi/trees/lib/FHeap.hpp \
	src/zi/tri_strip_to_obj.hpp \
	src/zi/utility.h \
	src/zi/watershed/MemMap.hpp \
	src/zi/watershed/RawQuickieWS.h

SOURCES +=  \
	src/common/colors.cpp \
	src/common/common.cpp \
	src/coordinates/chunk.cpp \
	src/coordinates/dataCoord.cpp \
	src/coordinates/global.cpp \
	src/coordinates/norm.cpp \
	src/coordinates/screen.cpp \
	src/coordinates/screenSystem.cpp \
	src/coordinates/volumeSystem.cpp \
	src/coordinates/yaml.cpp \
	src/threads/threadPoolManager.cpp \
	src/thrift/FacebookService.cpp \
	src/thrift/fb303_constants.cpp \
	src/thrift/fb303_types.cpp \
	src/thrift/filesystem_constants.cpp \
	src/thrift/filesystem_types.cpp \
	src/thrift/server.cpp \
	src/thrift/server_constants.cpp \
	src/thrift/server_types.cpp \
	src/thrift/storage_manager.cpp \
	src/thrift/storage_server.cpp \
	src/utility/fileHelpers.cpp \
	src/utility/primeNumbers.cpp \
	src/utility/systemInformation.cpp \
	src/zi/rwlock_example.cpp \
	src/zi/watershed/RawQuickieWS.cpp

## end of section to be rewritten using Perl

server_h.target = lib/include/thrift/server.h
server_h.commands = external/libs/thrift/bin/thrift -r --out src/thrift --gen cpp if/server.thrift
server_h.depends = if/server.thrift
QMAKE_EXTRA_TARGETS += server_h

server_cpp.target = src/thrift/server.cpp
server_cpp.commands = external/libs/thrift/bin/thrift -r --out src/thrift --gen cpp if/server.thrift
server_cpp.depends = if/server.thrift
QMAKE_EXTRA_TARGETS += server_cpp

storage_server_h.target = lib/include/thrift/storage_server.h
storage_server_h.commands = external/libs/thrift/bin/thrift -r --out src/thrift --gen cpp if/filesystem.thrift
storage_server_h.depends = if/server.thrift
QMAKE_EXTRA_TARGETS += storage_server_h

storage_server_cpp.target = src/thrift/storage_server.cpp
storage_server_cpp.commands = external/libs/thrift/bin/thrift -r --out src/thrift --gen cpp if/filesystem.thrift
storage_server_cpp.depends = if/filesystem.thrift
QMAKE_EXTRA_TARGETS += storage_server_cpp

storage_manager_h.target = lib/include/thrift/storage_manager.h
storage_manager_h.commands = external/libs/thrift/bin/thrift -r --out src/thrift --gen cpp if/filesystem.thrift
storage_manager_h.depends = if/manager.thrift
QMAKE_EXTRA_TARGETS += storage_manager_h

storage_manager_cpp.target = src/thrift/storage_manager.cpp
storage_manager_cpp.commands = external/libs/thrift/bin/thrift -r --out src/thrift --gen cpp if/filesystem.thrift
storage_manager_cpp.depends = if/filesystem.thrift
QMAKE_EXTRA_TARGETS += storage_manager_cpp

php_server.target = php/packages/server/TServer.php
php_server.commands = mkdir -p php/packages; external/libs/thrift/bin/thrift -r --out php/packages --gen php if/server.thrift
php_server.depends = if/server.thrift
QMAKE_EXTRA_TARGETS += php_server

php.target = php
php.depends = php/packages/server/TServer.php
QMAKE_EXTRA_TARGETS += php

INCLUDEPATH = src include lib
INCLUDEPATH += external/zi_lib
INCLUDEPATH += include/yaml-cpp/include

OBJECTS_DIR = build
MOC_DIR = build
RCC_DIR = build
DESTDIR = bin
TARGET = omni.common

inst.path = ./lib/bin
inst.files = ./bin/*
inst.extra = /bin/rm -Rf lib/include/;\
	     cd src;\
	     find -regex \".*\.h\\\(pp\\\)?\" -print0 | cpio --null -pvd ../lib/include/;\
	     cd ../external/srcs/thrift-0.7.0/lib/php/src/;\
	     find -regex \".*\.php\" -print0 | cpio --null -pvd ../../../../../../php/;
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
# g++4new: QMAKE_CXXFLAGS += -Werror=return-type

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
