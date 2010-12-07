SOURCES += hdf5_merge.cpp
TEMPLATE = app
TARGET = hdf5_merge

INCLUDEPATH =  ../../../external/libs/HDF5/include
LIBS += ../../../external/libs/HDF5/lib/libhdf5.a

OBJECTS_DIR = build
MOC_DIR = build
RCC_DIR = build
DESTDIR = bin

