#############################################################################
# Makefile for building: libomni.common.a
# Generated by qmake (2.01a) (Qt 4.6.2) on: Fri Oct 7 18:45:14 2011
# Project:  omni.common.pro
# Template: lib
# Command: /usr/bin/qmake -unix -o Makefile omni.common.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = 
CFLAGS        = -pipe -g -fPIC $(DEFINES)
CXXFLAGS      = -pipe -Werror=return-type -DZI_USE_OPENMP -fopenmp -gstabs+ -DBOOST_MULTI_INDEX_DISABLE_SERIALIZATION -DBOOST_SPIRIT_THREADSAFE -DBOOST_SYSTEM_NO_DEPRECATED -DBOOST_FILESYSTEM_VERSION=3 -DBOOST_FILESYSTEM_NO_DEPRECATED -g -fPIC $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++ -I. -Isrc -Iinclude -Ilib -Iexternal/zi_lib -Iinclude/yaml-cpp/include -Iexternal/libs/Boost/include -Iexternal/libs/thrift/include/thrift
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/bin/qmake
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = build/

####### Files

SOURCES       = src/common/colors.cpp \
		src/common/common.cpp \
		src/common/gl.cpp \
		src/coordinates/chunkCoord.cpp \
		src/coordinates/dataCoord.cpp \
		src/coordinates/globalCoord.cpp \
		src/coordinates/normCoord.cpp \
		src/coordinates/screenCoord.cpp \
		src/coordinates/screenSystem.cpp \
		src/coordinates/volumeSystem.cpp \
		src/coordinates/yaml.cpp \
		src/thrift/server.cpp \
		src/thrift/server_constants.cpp \
		src/thrift/server_types.cpp \
		src/utility/fileHelpers.cpp \
		src/utility/primeNumbers.cpp \
		src/utility/systemInformation.cpp \
		src/zi/watershed/RawQuickieWS.cpp 
OBJECTS       = build/colors.o \
		build/common.o \
		build/gl.o \
		build/chunkCoord.o \
		build/dataCoord.o \
		build/globalCoord.o \
		build/normCoord.o \
		build/screenCoord.o \
		build/screenSystem.o \
		build/volumeSystem.o \
		build/yaml.o \
		build/server.o \
		build/server_constants.o \
		build/server_types.o \
		build/fileHelpers.o \
		build/primeNumbers.o \
		build/systemInformation.o \
		build/RawQuickieWS.o
DIST          = /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/staticlib.prf \
		/usr/share/qt4/mkspecs/features/static.prf \
		omni.common.pro
QMAKE_TARGET  = omni.common
DESTDIR       = bin/
TARGET        = libomni.common.a

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile bin/$(TARGET) 

staticlib: bin/$(TARGET)

bin/$(TARGET):  $(OBJECTS) $(OBJCOMP) 
	@$(CHK_DIR_EXISTS) bin/ || $(MKDIR) bin/ 
	-$(DEL_FILE) $(TARGET)
	$(AR) $(TARGET) $(OBJECTS)
	-$(DEL_FILE) bin/$(TARGET)
	-$(MOVE) $(TARGET) bin/


Makefile: omni.common.pro  /usr/share/qt4/mkspecs/linux-g++/qmake.conf /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/staticlib.prf \
		/usr/share/qt4/mkspecs/features/static.prf
	$(QMAKE) -unix -o Makefile omni.common.pro
/usr/share/qt4/mkspecs/common/g++.conf:
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/debug.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/staticlib.prf:
/usr/share/qt4/mkspecs/features/static.prf:
qmake:  FORCE
	@$(QMAKE) -unix -o Makefile omni.common.pro

dist: 
	@$(CHK_DIR_EXISTS) build/omni.common1.0.0 || $(MKDIR) build/omni.common1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) build/omni.common1.0.0/ && (cd `dirname build/omni.common1.0.0` && $(TAR) omni.common1.0.0.tar omni.common1.0.0 && $(COMPRESS) omni.common1.0.0.tar) && $(MOVE) `dirname build/omni.common1.0.0`/omni.common1.0.0.tar.gz . && $(DEL_FILE) -r build/omni.common1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


src/thrift/server.h: if/server.thrift
	external/libs/thrift/bin/thrift -r --out src/thrift --gen cpp if/server.thrift

src/thrift/server_constants.h: if/server.thrift
	external/libs/thrift/bin/thrift -r --out src/thrift --gen cpp if/server.thrift

src/thrift/server_types.h: if/server.thrift
	external/libs/thrift/bin/thrift -r --out src/thrift --gen cpp if/server.thrift

src/thrift/server.h: if/server.thrift
	external/libs/thrift/bin/thrift -r --out src/thrift --gen cpp if/server.thrift

src/thrift/server_constants.h: if/server.thrift
	external/libs/thrift/bin/thrift -r --out src/thrift --gen cpp if/server.thrift

src/thrift/server_types.h: if/server.thrift
	external/libs/thrift/bin/thrift -r --out src/thrift --gen cpp if/server.thrift

src/thrift/server.cpp: if/server.thrift
	external/libs/thrift/bin/thrift -r --out src/thrift --gen cpp if/server.thrift

src/thrift/server.h: if/server.thrift
	external/libs/thrift/bin/thrift -r --out src/thrift --gen cpp if/server.thrift

src/thrift/server_constants.cpp: if/server.thrift
	external/libs/thrift/bin/thrift -r --out src/thrift --gen cpp if/server.thrift

src/thrift/server_constants.h: if/server.thrift
	external/libs/thrift/bin/thrift -r --out src/thrift --gen cpp if/server.thrift

src/thrift/server_types.cpp: if/server.thrift
	external/libs/thrift/bin/thrift -r --out src/thrift --gen cpp if/server.thrift

src/thrift/server_types.h: if/server.thrift
	external/libs/thrift/bin/thrift -r --out src/thrift --gen cpp if/server.thrift

compiler_clean: 

####### Compile

build/colors.o: src/common/colors.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/colors.o src/common/colors.cpp

build/common.o: src/common/common.cpp src/common/string.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/common.o src/common/common.cpp

build/gl.o: src/common/gl.cpp src/common/gl.h \
		src/common/std.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/gl.o src/common/gl.cpp

build/chunkCoord.o: src/coordinates/chunkCoord.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/chunkCoord.o src/coordinates/chunkCoord.cpp

build/dataCoord.o: src/coordinates/dataCoord.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/dataCoord.o src/coordinates/dataCoord.cpp

build/globalCoord.o: src/coordinates/globalCoord.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/globalCoord.o src/coordinates/globalCoord.cpp

build/normCoord.o: src/coordinates/normCoord.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/normCoord.o src/coordinates/normCoord.cpp

build/screenCoord.o: src/coordinates/screenCoord.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/screenCoord.o src/coordinates/screenCoord.cpp

build/screenSystem.o: src/coordinates/screenSystem.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/screenSystem.o src/coordinates/screenSystem.cpp

build/volumeSystem.o: src/coordinates/volumeSystem.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/volumeSystem.o src/coordinates/volumeSystem.cpp

build/yaml.o: src/coordinates/yaml.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/yaml.o src/coordinates/yaml.cpp

build/server.o: src/thrift/server.cpp src/thrift/server.h \
		src/thrift/server_types.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/server.o src/thrift/server.cpp

build/server_constants.o: src/thrift/server_constants.cpp src/thrift/server_constants.h \
		src/thrift/server_types.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/server_constants.o src/thrift/server_constants.cpp

build/server_types.o: src/thrift/server_types.cpp src/thrift/server_types.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/server_types.o src/thrift/server_types.cpp

build/fileHelpers.o: src/utility/fileHelpers.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/fileHelpers.o src/utility/fileHelpers.cpp

build/primeNumbers.o: src/utility/primeNumbers.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/primeNumbers.o src/utility/primeNumbers.cpp

build/systemInformation.o: src/utility/systemInformation.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/systemInformation.o src/utility/systemInformation.cpp

build/RawQuickieWS.o: src/zi/watershed/RawQuickieWS.cpp src/zi/watershed/RawQuickieWS.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/RawQuickieWS.o src/zi/watershed/RawQuickieWS.cpp

####### Install

install_inst: first FORCE
	@$(CHK_DIR_EXISTS) $(INSTALL_ROOT)/Users/balkamm/omni.common/lib/bin/ || $(MKDIR) $(INSTALL_ROOT)/Users/balkamm/omni.common/lib/bin/ 
	cd src; find -regex ".*\.h\(pp\)?" -print0 | cpio --null -pvd ../lib/include/
	-$(INSTALL_FILE) /Users/balkamm/omni.common/bin/libomni.common.a $(INSTALL_ROOT)/Users/balkamm/omni.common/lib/bin/


uninstall_inst:  FORCE
	-$(DEL_FILE) -r $(INSTALL_ROOT)/Users/balkamm/omni.common/lib/bin/libomni.common.a
	-$(DEL_DIR) $(INSTALL_ROOT)/Users/balkamm/omni.common/lib/bin/ 


install:  install_inst  FORCE

uninstall: uninstall_inst   FORCE

FORCE:

