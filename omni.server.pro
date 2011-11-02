TEMPLATE = app
CONFIG = console

!exists($$[COMMON_PATH]/omni.common.pro) {
   error(unable to find omni.common.  Please set the path using: qmake -set COMMON_PATH [path to omni.common])
}

## start of section to be rewritten using Perl
HEADERS +=  \
	src/chunks/chunk.h \
	src/chunks/chunkData.hpp \
	src/chunks/chunkDataImpl.hpp \
	src/chunks/chunkDataInterface.hpp \
	src/chunks/chunkItemContainer.hpp \
	src/chunks/chunkItemContainerMatrix.hpp \
	src/chunks/chunkMipping.hpp \
	src/chunks/chunkUtils.hpp \
	src/chunks/details/ptrToChunkDataBase.hpp \
	src/chunks/details/ptrToChunkDataMemMapVol.h \
	src/chunks/extractChanTile.hpp \
	src/chunks/extractSegTile.hpp \
	src/chunks/rawChunk.hpp \
	src/chunks/rawChunkMemMapped.hpp \
	src/chunks/rawChunkSlicer.hpp \
	src/chunks/segChunk.h \
	src/chunks/segChunkData.hpp \
	src/chunks/segChunkDataImpl.hpp \
	src/chunks/segChunkDataInterface.hpp \
	src/chunks/uniqueValues/chunkUniqueValuesManager.hpp \
	src/chunks/uniqueValues/chunkUniqueValuesPerThreshold.hpp \
	src/chunks/uniqueValues/chunkUniqueValuesTypes.h \
	src/chunks/uniqueValues/thresholdsInChunk.hpp \
	src/datalayer/IDataVolume.hpp \
	src/datalayer/archive/baseTypes.hpp \
	src/datalayer/archive/channel.h \
	src/datalayer/archive/filter.h \
	src/datalayer/archive/genericManager.hpp \
	src/datalayer/archive/mipVolume.hpp \
	src/datalayer/archive/project.h \
	src/datalayer/archive/segmentation.h \
	src/datalayer/dataPath.h \
	src/datalayer/dataPaths.h \
	src/datalayer/dataWrapper.h \
	src/datalayer/fs/IOnDiskFile.h \
	src/datalayer/fs/compressedFile.h \
	src/datalayer/fs/file.h \
	src/datalayer/fs/fileNames.hpp \
	src/datalayer/fs/fileQT.hpp \
	src/datalayer/fs/memMapCompressedFile.hpp \
	src/datalayer/fs/memMappedFileQT.hpp \
	src/datalayer/fs/memMappedFileQTNew.hpp \
	src/mesh/io/chunk/memMappedAllocFile.hpp \
	src/mesh/io/chunk/meshChunkAllocTable.h \
	src/mesh/io/chunk/meshChunkDataReader.hpp \
	src/mesh/io/chunk/meshChunkDataWriter.hpp \
	src/mesh/io/chunk/meshChunkDataWriterTask.hpp \
	src/mesh/io/chunk/meshChunkTypes.h \
	src/mesh/io/dataForMeshLoad.hpp \
	src/mesh/io/meshFilePtrCache.hpp \
	src/mesh/io/meshMetadata.hpp \
	src/mesh/io/meshReader.hpp \
	src/mesh/io/meshWriter.hpp \
	src/mesh/io/ringBuffer.hpp \
	src/mesh/io/threads/meshWriterTask.hpp \
	src/mesh/mesh.h \
	src/mesh/meshCoord.h \
	src/mesh/meshManager.h \
	src/mesh/meshManagers.hpp \
	src/mesh/meshParams.hpp \
	src/mesh/meshTypes.h \
	src/mesh/mesher/MeshCollector.hpp \
	src/mesh/mesher/TriStripCollector.hpp \
	src/mesh/mesher/mesherSetup.hpp \
	src/mesh/mesher/ziMesher.hpp \
	src/project/details/affinityGraphManager.h \
	src/project/details/channelManager.h \
	src/project/details/projectVolumes.h \
	src/project/details/segmentationManager.h \
	src/project/project.h \
	src/project/projectGlobals.h \
	src/project/projectImpl.hpp \
	src/segment/io/segmentListTypePage.hpp \
	src/segment/io/segmentPage.hpp \
	src/segment/io/segmentPageObjects.hpp \
	src/segment/io/segmentPageV4.hpp \
	src/segment/io/validGroupNum.hpp \
	src/segment/lists/segmentList.h \
	src/segment/lists/segmentListByMRU.h \
	src/segment/lists/segmentListGlobal.hpp \
	src/segment/lists/segmentListLowLevel.hpp \
	src/segment/lists/segmentListsTypes.h \
	src/segment/lowLevel/dynamicForestCache.hpp \
	src/segment/lowLevel/dynamicForestPool.hpp \
	src/segment/lowLevel/enabledSegments.hpp \
	src/segment/lowLevel/pagingPtrStore.h \
	src/segment/lowLevel/segmentChildren.hpp \
	src/segment/lowLevel/segmentGraph.h \
	src/segment/lowLevel/segmentGraphInitialLoad.hpp \
	src/segment/lowLevel/segmentLowLevelTypes.h \
	src/segment/lowLevel/segmentsImplLowLevel.h \
	src/segment/lowLevel/store/segmentStore.h \
	src/segment/segment.h \
	src/segment/segmentChildrenTypes.h \
	src/segment/segmentIterator.h \
	src/segment/segmentPointers.h \
	src/segment/segmentTypes.h \
	src/segment/segments.h \
	src/segment/segmentsImpl.h \
	src/serverHandler.hpp \
	src/tiles/tile.h \
	src/tiles/tileCoord.h \
	src/volume/affinityChannel.h \
	src/volume/affinityGraph.h \
	src/volume/channel.h \
	src/volume/channelFolder.h \
	src/volume/channelImpl.h \
	src/volume/io/memMappedVolume.h \
	src/volume/io/memMappedVolume.hpp \
	src/volume/io/volumeData.h \
	src/volume/mipVolume.h \
	src/volume/segmentation.h \
	src/volume/segmentationFolder.h \
	src/volume/segmentationLoader.h \
	src/volume/simpleRawVol.hpp \
	src/volume/volumeTypes.h

SOURCES +=  \
	include/yaml-cpp/src/aliasmanager.cpp \
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
        include/libb64/src/cdecode.c \
        include/libb64/src/cencode.c \
	src/volume/volume.cpp \
	src/volume/volumeTypes.cpp \
	src/volume/channel.cpp \
	src/volume/channelImpl.cpp \
	src/volume/io/volumeData.cpp \
        src/volume/io/memMappedVolume.cpp\
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
        src/segment/segments.cpp \
        src/segment/segmentsImpl.cpp \
        src/segment/segment.cpp \
        src/segment/lowLevel/segmentsImplLowLevel.cpp \
        src/segment/lowLevel/store/segmentStore.cpp \
        src/segment/lowLevel/pagingPtrStore.cpp \
        src/chunks/chunk.cpp \
        src/chunks/segChunk.cpp \
#        src/tiles/tile.cpp \
#        src/jpeg/jpeg.cpp \
#        src/network/jpeg.cpp \
        src/pipeline/jpeg.cpp \
        src/main.cpp


## end of section to be rewritten using Perl

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
    error(please run 'bootstrap.pl 1' to install Boost)
}

#### thrift
exists(external/libs/thrift) {
    INCLUDEPATH += external/libs/thrift/include/thrift/

    LIBS += external/libs/thrift/lib/libthrift.a
} else {
    error(please run 'bootstrap.pl 2' to install thrift)
}

#### libjpeg
exists(external/libs/libjpeg) {
    INCLUDEPATH += external/libs/libjpeg/include

    LIBS += external/libs/libjpeg/lib/libturbojpeg.a
} else {
    error(please run 'bootstrap.pl 3' to install libjpeg)
}

#### libpng
exists(external/libs/libpng) {
    INCLUDEPATH += external/libs/libpng/include

    LIBS += external/libs/libpng/lib/libpng.a
} else {
    error(please run 'bootstrap.pl 4' to install libpng)
}
