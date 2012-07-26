# -*- Makefile -*-

HERE    	=       .
EXTERNAL	=	$(HERE)/external/libs
BINDIR		=	./bin
BUILDDIR	=	build
GENDIR		=	common/src/thrift

AT		=   @
DOLLAR  = 	$$

CD      =	$(AT)cd
CP      =	$(AT)cp
ECHO    =	$(AT)echo
CAT     =	$(AT)cat
IF      =	$(AT)if
LN      =	$(AT)ln
MKDIR   =	$(AT)mkdir
MV      =	$(AT)mv
SED     =	$(AT)sed
RM      =	$(AT)rm
TOUCH   =	$(AT)touch
CHMOD   =	$(AT)chmod
DATE    =   $(AT)date
PERL    =	$(AT)perl
AR      =	$(AT)ar
ARFLAGS =	rcs

CC     =	$(AT)gcc
CXX    =	$(AT)g++
THRIFT = 	$(AT)$(EXTERNAL)/thrift/bin/thrift
MOC	   =    $(AT)$(EXTERNAL)/qt/bin/moc
RCC	   =    $(AT)$(EXTERNAL)/qt/bin/rcc
FPIC   =	-fPIC

INCLUDES	=	-I$(HERE) \
				-I$(HERE)/common/src \
				-I$(HERE)/common/include \
				-I$(HERE)/common/include/yaml-cpp/include \
				-I$(HERE)/common/include/libb64/include \
				-I$(HERE)/server/src \
				-I$(HERE)/filesystem/src \
				-I$(HERE)/zi_lib \
				-I$(EXTERNAL)/thrift/include/thrift \
				-I$(EXTERNAL)/boost/include \
				-I$(EXTERNAL)/libjpeg/include \
				-I$(EXTERNAL)/libpng/include \

DESKTOPINCLUDES = -I$(HERE)/desktop/src \
				  -I$(HERE)/desktop/include \
				  -I$(HERE)/desktop/lib \
				  -I$(HERE)/desktop \
				  -I$(HERE)/common/include \
				  -I$(HERE)/common/include/yaml-cpp/include \
				  -I$(HERE)/zi_lib \
				  -I$(EXTERNAL)/libjpeg/include \
				  -I$(EXTERNAL)/boost/include \
				  -I$(EXTERNAL)/qt/include/Qt \
				  -I$(EXTERNAL)/qt/include/QtCore \
				  -I$(EXTERNAL)/qt/include/QtOpenGL \
				  -I$(EXTERNAL)/qt/include/QtGui \
				  -I$(EXTERNAL)/qt/include/QtNetwork \
				  -I$(EXTERNAL)/qt/include \
				  -I$(EXTERNAL)/hdf5/include

LIBS = $(EXTERNAL)/boost/lib/libboost_filesystem.a \
	   $(EXTERNAL)/boost/lib/libboost_iostreams.a \
	   $(EXTERNAL)/boost/lib/libboost_system.a \
	   $(EXTERNAL)/boost/lib/libboost_thread.a \
	   $(EXTERNAL)/boost/lib/libboost_regex.a \
	   $(EXTERNAL)/thrift/lib/libthrift.a \
	   $(EXTERNAL)/thrift/lib/libthriftnb.a \
	   $(EXTERNAL)/libjpeg/lib/libturbojpeg.a \
	   $(EXTERNAL)/libpng/lib/libpng.a \
	   -levent -lpthread -lrt -lz

DESKTOPLIBS = -L$(EXTERNAL)/qt/lib \
			  $(EXTERNAL)/boost/lib/libboost_filesystem.a \
	          $(EXTERNAL)/boost/lib/libboost_iostreams.a \
	          $(EXTERNAL)/boost/lib/libboost_system.a \
	          $(EXTERNAL)/boost/lib/libboost_thread.a \
	          $(EXTERNAL)/boost/lib/libboost_regex.a \
	          $(EXTERNAL)/libjpeg/lib/libturbojpeg.a \
	          $(EXTERNAL)/libpng/lib/libpng.a \
                  $(EXTERNAL)/hdf5/lib/libhdf5.a \
              -lQtGui \
              -lQtNetwork \
              -lQtCore \
              -lQtOpenGL \
              -lGLU \
              -lGL \
	   		  -levent -lpthread -lrt -lz

CXX_INCLUDES	=	$(INCLUDES)

CWARN		=	-Wall -Wno-sign-compare -Wno-unused-variable -Wno-return-type
CXXWARN		=	$(CWARN) -Wno-deprecated -Woverloaded-virtual

CPP_DEPFLAGS		=	-MM -MG -MP -MT "$(@:.d=.o)"
CPP_INLINE_DEPFLAGS	=	-MMD -MP -MT "$(@)" -MF $(@:.o=.T)
COMMON_CFLAGS		=	-g -std=gnu99 -D_GNU_SOURCE=1 \
				-D_REENTRANT $(CPP_INLINE_DEPFLAGS) \
				$(FPIC) $(CWARN)

THRIFT_CXXFLAGS	   = 	-DHAVE_CONFIG_H

COMMON_CXXFLAGS    =	-g $(CPP_INLINE_DEPFLAGS) \
						   $(FPIC) $(CXXWARN) $(THRIFT_CXXFLAGS)

DBG_CFLAGS         =	$(COMMON_CFLAGS) -DDEBUG_MODE=1
DBG_CXXFLAGS       =	$(COMMON_CXXFLAGS) -DDEBUG_MODE=1 -gstabs+
OPTIMIZATION_FLAGS =	-O3
OPT_CFLAGS         =	$(COMMON_CFLAGS) -DNDEBUG \
						$(OPTIMIZATION_FLAGS) -fno-omit-frame-pointer
OPT_CXXFLAGS       =	$(COMMON_CXXFLAGS) -DNDEBUG \
						$(OPTIMIZATION_FLAGS) -fno-omit-frame-pointer
COMMON_LDFLAGS     =	-g $(FPIC) -Wl,--eh-frame-hdr -lm
DBG_LDFLAGS        =	$(COMMON_LDFLAGS) -gstabs+
OPT_LDFLAGS        =	$(COMMON_LDFLAGS) -O3 -fno-omit-frame-pointer

COMM_FLEX_FLAGS    =    -d
OPT_FLEXFLAGS      =    $(COMM_FLEX_FLAGS)
DBG_FLEXFLAGS      =    $(COMM_FLEX_FLAGS) -t

DEFINES = -DQT_NO_KEYWORDS -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB -DQT_SHARED \
-DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED -DBOOST_MULTI_INDEX_DISABLE_SERIALIZATION \
-DBOOST_FILESYSTEM_NO_DEPRECATED -DBOOST_FILESYSTEM_VERSION=3 -DBOOST_SYSTEM_NO_DEPRECATED

# Get rid of these if building with clang.
EXTRA_CXXFLAGS = -DZI_USE_OPENMP -fopenmp
EXTRA_LDFLAGS  = -DZI_USE_OPENMP -fopenmp

ifneq ($(strip $(OPT)),)
  CFLAGS	=	$(OPT_CFLAGS) $(EXTRA_CFLAGS)
  CXXFLAGS	=	$(OPT_CXXFLAGS) $(EXTRA_CXXFLAGS)
  LDFLAGS	=	$(OPT_LDFLAGS) $(EXTRA_LDFLAGS)
else
  CFLAGS	=	$(DBG_CFLAGS) $(EXTRA_CFLAGS)
  CXXFLAGS	=	$(DBG_CXXFLAGS) $(EXTRA_CXXFLAGS)
  LDFLAGS	=	$(DBG_LDFLAGS) $(EXTRA_LDFLAGS)
endif

define build_cpp
	$(ECHO) "[CXX] compiling $<"
	$(MKDIR) -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) -o $@ $<
	$(MV) -f "$(@:.o=.T)" "$(@:.o=.d)"
endef

define build_c
	$(ECHO) "[CC] compiling $<"
	$(MKDIR) -p $(dir $@)
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
endef

define make_d
	$(MKDIR) -p $(dir $@)
	$(CXX) $(CPP_DEPFLAGS) $(INCLUDES) -MF $@ $<
endef

THRIFT_DEPS = common/src/thrift/server.thrift.mkcpp \
			  common/src/thrift/filesystem.thrift.mkcpp

build/common/%.d: common/src/%.cpp $(THRIFT_DEPS)
	$(make_d)
build/common/%.o: common/src/%.cpp $(THRIFT_DEPS)
	$(build_cpp)

build/server/%.d: server/src/%.cpp $(THRIFT_DEPS)
	$(make_d)
build/server/%.o: server/src/%.cpp $(THRIFT_DEPS)
	$(build_cpp)

build/desktop/%.d: desktop/src/%.cpp
	$(MKDIR) -p $(dir $@)
	$(CXX) $(CPP_DEPFLAGS) $(DESKTOPINCLUDES) -MF $@ $<
build/desktop/%.o: desktop/src/%.cpp
	$(ECHO) "[CXX] compiling $<"
	$(MKDIR) -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) $(DESKTOPINCLUDES) $(DEFINES) -Wno-unused-but-set-variable -o $@ $<
	$(MV) -f "$(@:.o=.T)" "$(@:.o=.d)"

build/filesystem/%.d: filesystem/src/%.cpp
	$(make_d)
build/filesystem/%.o: filesystem/src/%.cpp
	$(build_cpp)

%.d: %.cpp
	$(make_d)
%.o: %.cpp
	$(build_cpp)
%.o: %.c
	$(build_c)
%.moc.cpp: %.hpp
	$(ECHO) "[MOC] Generating $<"
	$(MKDIR) -p $(dir $@)
	$(MOC) $(DEFINES) $(DESKTOPINCLUDES) -o $@ $<
%.moc.cpp: %.h
	$(ECHO) "[MOC] Generating $<"
	$(MKDIR) -p $(dir $@)
	$(MOC) $(DEFINES) $(DESKTOPINCLUDES) -o $@ $<
%.rcc.cpp: %.qrc
	$(ECHO) "[RCC] Generating $@"
	$(MKDIR) -p $(dir $@)
	$(RCC) -name $(basename $(notdir $<)) $< -o $@

common/src/thrift/%.thrift.mkcpp: common/if/%.thrift
	$(ECHO) "[Thrift] Generating $@"
	$(MKDIR) -p $(dir $@)
	$(TOUCH) $@.tmp
	$(THRIFT) -r --out common/src/thrift --gen cpp $<
	$(RM) common/src/thrift/*.skeleton.cpp

	$(MV) $@.tmp $@

.PHONY: all
all: $(BINDIR)/omni.server $(BINDIR)/omni.desktop

.PHONY: tidy
tidy:
	$(ECHO) Tidy...
	$(AT)touch .tmp.tmp~
	$(AT)touch ./tmp.tmp#
	$(AT)find . | grep "~$$" | xargs rm -rf
	$(AT)find . | grep "#"   | xargs rm -rf

.PHONY: clean
clean:
	$(ECHO) Cleaning...
	$(RM) -rf $(BINDIR) $(GENDIR) $(BUILDDIR)

.PHONY: remake
remake: clean all

COMMONSOURCES     = $(subst common/src,build/common, 				\
					  $(shell find common/src -iname "*.cpp"))

SERVERSOURCES     = $(subst server/src,build/server, 				\
                      $(shell find server/src -iname "*.cpp"))

DESKTOPSOURCES    = $(subst desktop/src,build/desktop, 				\
                      $(shell find desktop/src -iname "*.cpp"))

DESKTOPHEADERS    = $(subst desktop/src,build/desktop, 				\
                      $(shell grep Q_OBJECT -R desktop/src | cut -f1 -d ':'))

YAMLSOURCES = $(shell find common/include/yaml-cpp/src -iname "*.cpp" )
LIB64SOURCES = common/include/libb64/src/cencode.o

SERVER_SRCS = $(COMMONSOURCES) $(SERVERSOURCES) $(YAMLSOURCES) $(LIB64SOURCES)
SERVER_DEPS := $(SERVER_SRCS:.cpp=.o)

OMNI_SRCS = $(DESKTOPSOURCES)
MOC_SRCS = $(DESKTOPHEADERS:.hpp=.moc.cpp)
MOC_SRCS2 = $(MOC_SRCS:.h=.moc.cpp)

OMNI_DEPS := $(OMNI_SRCS:.cpp=.o) $(MOC_SRCS2:.cpp=.o) $(YAMLSOURCES:.cpp=.o)

define link
	$(ECHO) "[CXX] linking $@"
	$(MKDIR) -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $(filter-out %.mkcpp,$^) $(LIBS)
endef

$(BINDIR)/omni.server: $(SERVER_DEPS) $(THRIFT_DEPS)
	$(link)

$(BINDIR)/omni.desktop: $(OMNI_DEPS) desktop/lib/strnatcmp.o build/desktop/gui/resources.rcc.o
	$(ECHO) "[CXX] linking $@"
	$(MKDIR) -p $(dir $@)
	$(CXX) $(CXXFLAGS) -Wl,-rpath='$$ORIGIN../external/libs/qt/lib' -o $@ $(filter-out %.mkcpp,$^) $(DESKTOPLIBS)

ALLDEPS = $(shell find build -iname "*.d")

-include $(ALLDEPS)