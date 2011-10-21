#############################################################################
# Makefile for building: bin/omni.server
# Generated by qmake (2.01a) (Qt 4.6.2) on: Fri Oct 21 11:50:28 2011
# Project:  omni.server.pro
# Template: app
# Command: /usr/bin/qmake -unix -o Makefile omni.server.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = 
CFLAGS        = -pipe -g $(DEFINES)
CXXFLAGS      = -pipe -Werror=return-type -DZI_USE_OPENMP -fopenmp -gstabs+ -DBOOST_MULTI_INDEX_DISABLE_SERIALIZATION -DBOOST_SPIRIT_THREADSAFE -DBOOST_SYSTEM_NO_DEPRECATED -DBOOST_FILESYSTEM_VERSION=3 -DBOOST_FILESYSTEM_NO_DEPRECATED -g $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++ -I. -Isrc -Iinclude -Ilib -Iexternal/zi_lib -Iinclude/yaml-cpp/include -I../omni.common/lib/include -Iexternal/libs/Boost/include -Iexternal/libs/thrift/include/thrift
LINK          = g++
LFLAGS        = -DZI_USE_OPENMP -fopenmp -gstabs+
LIBS          = $(SUBLIBS)   ../omni.common/lib/bin/libomni.common.a external/libs/Boost/lib/libboost_filesystem.a external/libs/Boost/lib/libboost_iostreams.a external/libs/Boost/lib/libboost_system.a external/libs/Boost/lib/libboost_thread.a external/libs/thrift/lib/libthrift.a 
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

SOURCES       = include/yaml-cpp/src/aliasmanager.cpp \
		include/yaml-cpp/src/contrib/graphbuilder.cpp \
		include/yaml-cpp/src/contrib/graphbuilderadapter.cpp \
		include/yaml-cpp/src/conversion.cpp \
		include/yaml-cpp/src/directives.cpp \
		include/yaml-cpp/src/emitfromevents.cpp \
		include/yaml-cpp/src/emitter.cpp \
		include/yaml-cpp/src/emitterstate.cpp \
		include/yaml-cpp/src/emitterutils.cpp \
		include/yaml-cpp/src/exp.cpp \
		include/yaml-cpp/src/iterator.cpp \
		include/yaml-cpp/src/node.cpp \
		include/yaml-cpp/src/nodebuilder.cpp \
		include/yaml-cpp/src/nodeownership.cpp \
		include/yaml-cpp/src/null.cpp \
		include/yaml-cpp/src/ostream.cpp \
		include/yaml-cpp/src/parser.cpp \
		include/yaml-cpp/src/regex.cpp \
		include/yaml-cpp/src/scanner.cpp \
		include/yaml-cpp/src/scanscalar.cpp \
		include/yaml-cpp/src/scantag.cpp \
		include/yaml-cpp/src/scantoken.cpp \
		include/yaml-cpp/src/simplekey.cpp \
		include/yaml-cpp/src/singledocparser.cpp \
		include/yaml-cpp/src/stream.cpp \
		include/yaml-cpp/src/tag.cpp \
		src/volume/volume.cpp \
		src/volume/volumeTypes.cpp \
		src/volume/channel.cpp \
		src/volume/channelImpl.cpp \
		src/volume/io/volumeData.cpp \
		src/volume/io/memMappedVolume.cpp \
		src/volume/channelFolder.cpp \
		src/volume/segmentation.cpp \
		src/volume/segmentationFolder.cpp \
		src/project/project.cpp \
		src/project/details/projectVolumes.cpp \
		src/project/details/segmentationManager.cpp \
		src/project/details/channelManager.cpp \
		src/datalayer/fs/file.cpp \
		src/datalayer/archive/projectYaml.cpp \
		src/datalayer/archive/channelYaml.cpp \
		src/datalayer/archive/segmentationYaml.cpp \
		src/chunks/chunk.cpp \
		src/chunks/segChunk.cpp \
		src/main.cpp 
OBJECTS       = build/aliasmanager.o \
		build/graphbuilder.o \
		build/graphbuilderadapter.o \
		build/conversion.o \
		build/directives.o \
		build/emitfromevents.o \
		build/emitter.o \
		build/emitterstate.o \
		build/emitterutils.o \
		build/exp.o \
		build/iterator.o \
		build/node.o \
		build/nodebuilder.o \
		build/nodeownership.o \
		build/null.o \
		build/ostream.o \
		build/parser.o \
		build/regex.o \
		build/scanner.o \
		build/scanscalar.o \
		build/scantag.o \
		build/scantoken.o \
		build/simplekey.o \
		build/singledocparser.o \
		build/stream.o \
		build/tag.o \
		build/volume.o \
		build/volumeTypes.o \
		build/channel.o \
		build/channelImpl.o \
		build/volumeData.o \
		build/memMappedVolume.o \
		build/channelFolder.o \
		build/segmentation.o \
		build/segmentationFolder.o \
		build/project.o \
		build/projectVolumes.o \
		build/segmentationManager.o \
		build/channelManager.o \
		build/file.o \
		build/projectYaml.o \
		build/channelYaml.o \
		build/segmentationYaml.o \
		build/chunk.o \
		build/segChunk.o \
		build/main.o
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
		omni.server.pro
QMAKE_TARGET  = omni.server
DESTDIR       = bin/
TARGET        = bin/omni.server

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

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	@$(CHK_DIR_EXISTS) bin/ || $(MKDIR) bin/ 
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: omni.server.pro  /usr/share/qt4/mkspecs/linux-g++/qmake.conf /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf
	$(QMAKE) -unix -o Makefile omni.server.pro
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
qmake:  FORCE
	@$(QMAKE) -unix -o Makefile omni.server.pro

dist: 
	@$(CHK_DIR_EXISTS) build/omni.server1.0.0 || $(MKDIR) build/omni.server1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) build/omni.server1.0.0/ && (cd `dirname build/omni.server1.0.0` && $(TAR) omni.server1.0.0.tar omni.server1.0.0 && $(COMPRESS) omni.server1.0.0.tar) && $(MOVE) `dirname build/omni.server1.0.0`/omni.server1.0.0.tar.gz . && $(DEL_FILE) -r build/omni.server1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


compiler_clean: 

####### Compile

build/aliasmanager.o: include/yaml-cpp/src/aliasmanager.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/aliasmanager.o include/yaml-cpp/src/aliasmanager.cpp

build/graphbuilder.o: include/yaml-cpp/src/contrib/graphbuilder.cpp include/yaml-cpp/src/contrib/graphbuilderadapter.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/graphbuilder.o include/yaml-cpp/src/contrib/graphbuilder.cpp

build/graphbuilderadapter.o: include/yaml-cpp/src/contrib/graphbuilderadapter.cpp include/yaml-cpp/src/contrib/graphbuilderadapter.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/graphbuilderadapter.o include/yaml-cpp/src/contrib/graphbuilderadapter.cpp

build/conversion.o: include/yaml-cpp/src/conversion.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/conversion.o include/yaml-cpp/src/conversion.cpp

build/directives.o: include/yaml-cpp/src/directives.cpp include/yaml-cpp/src/directives.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/directives.o include/yaml-cpp/src/directives.cpp

build/emitfromevents.o: include/yaml-cpp/src/emitfromevents.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/emitfromevents.o include/yaml-cpp/src/emitfromevents.cpp

build/emitter.o: include/yaml-cpp/src/emitter.cpp include/yaml-cpp/src/emitterstate.h \
		include/yaml-cpp/src/ptr_stack.h \
		include/yaml-cpp/src/setting.h \
		include/yaml-cpp/src/emitterutils.h \
		include/yaml-cpp/src/indentation.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/emitter.o include/yaml-cpp/src/emitter.cpp

build/emitterstate.o: include/yaml-cpp/src/emitterstate.cpp include/yaml-cpp/src/emitterstate.h \
		include/yaml-cpp/src/ptr_stack.h \
		include/yaml-cpp/src/setting.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/emitterstate.o include/yaml-cpp/src/emitterstate.cpp

build/emitterutils.o: include/yaml-cpp/src/emitterutils.cpp include/yaml-cpp/src/emitterutils.h \
		include/yaml-cpp/src/exp.h \
		include/yaml-cpp/src/regex.h \
		include/yaml-cpp/src/regeximpl.h \
		include/yaml-cpp/src/stream.h \
		include/yaml-cpp/src/stringsource.h \
		include/yaml-cpp/src/streamcharsource.h \
		include/yaml-cpp/src/indentation.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/emitterutils.o include/yaml-cpp/src/emitterutils.cpp

build/exp.o: include/yaml-cpp/src/exp.cpp include/yaml-cpp/src/exp.h \
		include/yaml-cpp/src/regex.h \
		include/yaml-cpp/src/regeximpl.h \
		include/yaml-cpp/src/stream.h \
		include/yaml-cpp/src/stringsource.h \
		include/yaml-cpp/src/streamcharsource.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/exp.o include/yaml-cpp/src/exp.cpp

build/iterator.o: include/yaml-cpp/src/iterator.cpp include/yaml-cpp/src/iterpriv.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/iterator.o include/yaml-cpp/src/iterator.cpp

build/node.o: include/yaml-cpp/src/node.cpp include/yaml-cpp/src/iterpriv.h \
		include/yaml-cpp/src/nodebuilder.h \
		include/yaml-cpp/src/nodeownership.h \
		include/yaml-cpp/src/ptr_vector.h \
		include/yaml-cpp/src/scanner.h \
		include/yaml-cpp/src/stream.h \
		include/yaml-cpp/src/token.h \
		include/yaml-cpp/src/tag.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/node.o include/yaml-cpp/src/node.cpp

build/nodebuilder.o: include/yaml-cpp/src/nodebuilder.cpp include/yaml-cpp/src/nodebuilder.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/nodebuilder.o include/yaml-cpp/src/nodebuilder.cpp

build/nodeownership.o: include/yaml-cpp/src/nodeownership.cpp include/yaml-cpp/src/nodeownership.h \
		include/yaml-cpp/src/ptr_vector.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/nodeownership.o include/yaml-cpp/src/nodeownership.cpp

build/null.o: include/yaml-cpp/src/null.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/null.o include/yaml-cpp/src/null.cpp

build/ostream.o: include/yaml-cpp/src/ostream.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/ostream.o include/yaml-cpp/src/ostream.cpp

build/parser.o: include/yaml-cpp/src/parser.cpp include/yaml-cpp/src/directives.h \
		include/yaml-cpp/src/nodebuilder.h \
		include/yaml-cpp/src/scanner.h \
		include/yaml-cpp/src/ptr_vector.h \
		include/yaml-cpp/src/stream.h \
		include/yaml-cpp/src/token.h \
		include/yaml-cpp/src/singledocparser.h \
		include/yaml-cpp/src/tag.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/parser.o include/yaml-cpp/src/parser.cpp

build/regex.o: include/yaml-cpp/src/regex.cpp include/yaml-cpp/src/regex.h \
		include/yaml-cpp/src/regeximpl.h \
		include/yaml-cpp/src/stream.h \
		include/yaml-cpp/src/stringsource.h \
		include/yaml-cpp/src/streamcharsource.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/regex.o include/yaml-cpp/src/regex.cpp

build/scanner.o: include/yaml-cpp/src/scanner.cpp include/yaml-cpp/src/scanner.h \
		include/yaml-cpp/src/ptr_vector.h \
		include/yaml-cpp/src/stream.h \
		include/yaml-cpp/src/token.h \
		include/yaml-cpp/src/exp.h \
		include/yaml-cpp/src/regex.h \
		include/yaml-cpp/src/regeximpl.h \
		include/yaml-cpp/src/stringsource.h \
		include/yaml-cpp/src/streamcharsource.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/scanner.o include/yaml-cpp/src/scanner.cpp

build/scanscalar.o: include/yaml-cpp/src/scanscalar.cpp include/yaml-cpp/src/scanscalar.h \
		include/yaml-cpp/src/regex.h \
		include/yaml-cpp/src/regeximpl.h \
		include/yaml-cpp/src/stream.h \
		include/yaml-cpp/src/stringsource.h \
		include/yaml-cpp/src/streamcharsource.h \
		include/yaml-cpp/src/scanner.h \
		include/yaml-cpp/src/ptr_vector.h \
		include/yaml-cpp/src/token.h \
		include/yaml-cpp/src/exp.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/scanscalar.o include/yaml-cpp/src/scanscalar.cpp

build/scantag.o: include/yaml-cpp/src/scantag.cpp include/yaml-cpp/src/scanner.h \
		include/yaml-cpp/src/ptr_vector.h \
		include/yaml-cpp/src/stream.h \
		include/yaml-cpp/src/token.h \
		include/yaml-cpp/src/regex.h \
		include/yaml-cpp/src/regeximpl.h \
		include/yaml-cpp/src/stringsource.h \
		include/yaml-cpp/src/streamcharsource.h \
		include/yaml-cpp/src/exp.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/scantag.o include/yaml-cpp/src/scantag.cpp

build/scantoken.o: include/yaml-cpp/src/scantoken.cpp include/yaml-cpp/src/scanner.h \
		include/yaml-cpp/src/ptr_vector.h \
		include/yaml-cpp/src/stream.h \
		include/yaml-cpp/src/token.h \
		include/yaml-cpp/src/exp.h \
		include/yaml-cpp/src/regex.h \
		include/yaml-cpp/src/regeximpl.h \
		include/yaml-cpp/src/stringsource.h \
		include/yaml-cpp/src/streamcharsource.h \
		include/yaml-cpp/src/scanscalar.h \
		include/yaml-cpp/src/scantag.h \
		include/yaml-cpp/src/tag.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/scantoken.o include/yaml-cpp/src/scantoken.cpp

build/simplekey.o: include/yaml-cpp/src/simplekey.cpp include/yaml-cpp/src/scanner.h \
		include/yaml-cpp/src/ptr_vector.h \
		include/yaml-cpp/src/stream.h \
		include/yaml-cpp/src/token.h \
		include/yaml-cpp/src/exp.h \
		include/yaml-cpp/src/regex.h \
		include/yaml-cpp/src/regeximpl.h \
		include/yaml-cpp/src/stringsource.h \
		include/yaml-cpp/src/streamcharsource.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/simplekey.o include/yaml-cpp/src/simplekey.cpp

build/singledocparser.o: include/yaml-cpp/src/singledocparser.cpp include/yaml-cpp/src/singledocparser.h \
		include/yaml-cpp/src/collectionstack.h \
		include/yaml-cpp/src/directives.h \
		include/yaml-cpp/src/scanner.h \
		include/yaml-cpp/src/ptr_vector.h \
		include/yaml-cpp/src/stream.h \
		include/yaml-cpp/src/token.h \
		include/yaml-cpp/src/tag.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/singledocparser.o include/yaml-cpp/src/singledocparser.cpp

build/stream.o: include/yaml-cpp/src/stream.cpp include/yaml-cpp/src/stream.h \
		include/yaml-cpp/src/exp.h \
		include/yaml-cpp/src/regex.h \
		include/yaml-cpp/src/regeximpl.h \
		include/yaml-cpp/src/stringsource.h \
		include/yaml-cpp/src/streamcharsource.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/stream.o include/yaml-cpp/src/stream.cpp

build/tag.o: include/yaml-cpp/src/tag.cpp include/yaml-cpp/src/tag.h \
		include/yaml-cpp/src/directives.h \
		include/yaml-cpp/src/token.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/tag.o include/yaml-cpp/src/tag.cpp

build/volume.o: src/volume/volume.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/volume.o src/volume/volume.cpp

build/volumeTypes.o: src/volume/volumeTypes.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/volumeTypes.o src/volume/volumeTypes.cpp

build/channel.o: src/volume/channel.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/channel.o src/volume/channel.cpp

build/channelImpl.o: src/volume/channelImpl.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/channelImpl.o src/volume/channelImpl.cpp

build/volumeData.o: src/volume/io/volumeData.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/volumeData.o src/volume/io/volumeData.cpp

build/memMappedVolume.o: src/volume/io/memMappedVolume.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/memMappedVolume.o src/volume/io/memMappedVolume.cpp

build/channelFolder.o: src/volume/channelFolder.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/channelFolder.o src/volume/channelFolder.cpp

build/segmentation.o: src/volume/segmentation.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/segmentation.o src/volume/segmentation.cpp

build/segmentationFolder.o: src/volume/segmentationFolder.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/segmentationFolder.o src/volume/segmentationFolder.cpp

build/project.o: src/project/project.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/project.o src/project/project.cpp

build/projectVolumes.o: src/project/details/projectVolumes.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/projectVolumes.o src/project/details/projectVolumes.cpp

build/segmentationManager.o: src/project/details/segmentationManager.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/segmentationManager.o src/project/details/segmentationManager.cpp

build/channelManager.o: src/project/details/channelManager.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/channelManager.o src/project/details/channelManager.cpp

build/file.o: src/datalayer/fs/file.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/file.o src/datalayer/fs/file.cpp

build/projectYaml.o: src/datalayer/archive/projectYaml.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/projectYaml.o src/datalayer/archive/projectYaml.cpp

build/channelYaml.o: src/datalayer/archive/channelYaml.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/channelYaml.o src/datalayer/archive/channelYaml.cpp

build/segmentationYaml.o: src/datalayer/archive/segmentationYaml.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/segmentationYaml.o src/datalayer/archive/segmentationYaml.cpp

build/chunk.o: src/chunks/chunk.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/chunk.o src/chunks/chunk.cpp

build/segChunk.o: src/chunks/segChunk.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/segChunk.o src/chunks/segChunk.cpp

build/main.o: src/main.cpp src/serverHandler.hpp \
		src/project/project.h \
		src/project/details/projectVolumes.h \
		src/project/details/channelManager.h \
		src/volume/channel.h \
		src/project/details/segmentationManager.h \
		src/volume/segmentation.h \
		src/tiles/tile.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/main.o src/main.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

