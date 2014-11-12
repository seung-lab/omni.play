SOURCES += h5meshget.cpp
TEMPLATE = app
TARGET = h5meshget

INCLUDEPATH =  ../../../external/libs/HDF5/include
LIBS += ../../../external/libs/HDF5/lib/libhdf5.a

OBJECTS_DIR = build
MOC_DIR = build
RCC_DIR = build
DESTDIR = bin

