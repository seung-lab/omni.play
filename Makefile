# -*- Makefile -*-

HERE    	=       .
BINDIR		=	./bin

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

CC   =	$(AT)gcc
CXX  =	$(AT)g++
FPIC =	-fPIC

INCLUDES	=	-I$(HERE) \
				-I$(HERE)/src \
				-I$(HERE)/external/include \
				-I$(HERE)/include \
				-Iexternal/zi_lib \
				-Iinclude/yaml-cpp/include \
				-Iinclude/libb64/include \
				-I../omni.common/lib/include \
				-Iexternal/srcs/thrift-0.7.0/contrib/fb303/cpp \
				-I../omni.common/lib/include/thrift \
				-Iexternal/libs/Boost/include \
				-Iexternal/libs/thrift/include/thrift \
				-Iexternal/libs/libjpeg/include \
				-Iexternal/libs/libpng/include \
				-Iexternal/libs/zlib/include

LIBS = ../omni.common/lib/bin/libomni.common.a \
	   external/libs/Boost/lib/libboost_filesystem.a \
	   external/libs/Boost/lib/libboost_iostreams.a \
	   external/libs/Boost/lib/libboost_system.a \
	   external/libs/Boost/lib/libboost_thread.a \
	   external/libs/Boost/lib/libboost_regex.a \
	   external/libs/thrift/lib/libthrift.a \
	   external/libs/thrift/lib/libthriftnb.a \
	   external/libs/libjpeg/lib/libturbojpeg.a \
	   external/libs/libpng/lib/libpng.a \
	   external/libs/zlib/lib/libz.a \
	   -levent -lpthread -levent -lrt 

CXX_INCLUDES	=	$(INCLUDES)

CWARN		=	-Wall -Wno-sign-compare -Wno-unused-variable -Wno-return-type
CXXWARN		=	$(CWARN) -Wno-deprecated -Woverloaded-virtual

CPP_DEPFLAGS		=	-MM -MG -MP $(CXX_INCLUDES) -MT "$(@:.d=.o)"
CPP_INLINE_DEPFLAGS	=	-MMD -MP -MT "$(@)" -MF $(@:.o=.T)
COMMON_CFLAGS		=	-g -std=gnu99 -D_GNU_SOURCE=1 \
				-D_REENTRANT $(CPP_INLINE_DEPFLAGS) \
				$(INCLUDES) $(FPIC) $(CWARN)

COMMON_CXXFLAGS    =	-g $(CPP_INLINE_DEPFLAGS) $(CXX_INCLUDES) \
						   $(FPIC) $(CXXWARN) -std=c++0x

DBG_CFLAGS         =	$(COMMON_CFLAGS) -DDEBUG_MODE=1
DBG_CXXFLAGS       =	$(COMMON_CXXFLAGS) -DDEBUG_MODE=1
OPTIMIZATION_FLAGS =	-O3
OPT_CFLAGS         =	$(COMMON_CFLAGS) -DNDEBUG \
						$(OPTIMIZATION_FLAGS) -fno-omit-frame-pointer
OPT_CXXFLAGS       =	$(COMMON_CXXFLAGS) -DNDEBUG \
						$(OPTIMIZATION_FLAGS) -fno-omit-frame-pointer
COMMON_LDFLAGS     =	-g $(FPIC) -Wl,--eh-frame-hdr -lm
DBG_LDFLAGS        =	$(COMMON_LDFLAGS)
OPT_LDFLAGS        =	$(COMMON_LDFLAGS) -O3 -fno-omit-frame-pointer

COMM_FLEX_FLAGS    =    -d
OPT_FLEXFLAGS      =    $(COMM_FLEX_FLAGS)
DBG_FLEXFLAGS      =    $(COMM_FLEX_FLAGS) -t

ifneq ($(strip $(OPT)),)
  CFLAGS	=	$(OPT_CFLAGS) $(EXTRA_CFLAGS)
  CXXFLAGS	=	$(OPT_CXXFLAGS) $(EXTRA_CXXFLAGS)
  LDFLAGS	=	$(OPT_LDFLAGS) $(EXTRA_LDFLAGS)
else
  CFLAGS	=	$(DBG_CFLAGS) $(EXTRA_CFLAGS)
  CXXFLAGS	=	$(DBG_CXXFLAGS) $(EXTRA_CXXFLAGS)
  LDFLAGS	=	$(DBG_LDFLAGS) $(EXTRA_LDFLAGS)
endif

# dependency files for c++
$(BINDIR)/%.d: src/%.cpp
	$(MKDIR) -p $(dir $@)
	$(CXX) $(CPP_DEPFLAGS) -MF $@ $<

# c++
$(BINDIR)/%.o: src/%.cpp
	$(ECHO) "[CXX] compiling $<"
	$(MKDIR) -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	$(MV) -f "$(@:.o=.T)" "$(@:.o=.d)"

# c
$(BINDIR)/%.o: src/%.c
	$(ECHO) "[CC] compiling $<"
	$(MKDIR) -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $<

# dependency files for c++
%.d: %.cpp
	$(MKDIR) -p $(dir $@)
	$(CXX) $(CPP_DEPFLAGS) -MF $@ $<

# c++
%.o: %.cpp
	$(ECHO) "[CXX] compiling $<"
	$(MKDIR) -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	$(MV) -f "$(@:.o=.T)" "$(@:.o=.d)"

# c
%.o: %.c
	$(ECHO) "[CC] compiling $<"
	$(MKDIR) -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $<


.PHONY: all
all:

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
	$(RM) -rf $(BINDIR) $(GENDIR)

.PHONY: remake
remake: clean all

SOURCES = $(shell find src | grep -Z --color=never ".cpp$$" | sed 's/\.cpp/\.o/g' | sed 's/src\//\.\/bin\//g' )
YAMLSOURCES = $(shell find include/yaml-cpp/src | grep -Z --color=never ".cpp$$" | sed 's/\.cpp/\.o/g' )
MISCSOURCES = include/libb64/src/cencode.o \
			  external/srcs/thrift-0.7.0/contrib/fb303/cpp/FacebookBase.o \
			  external/srcs/thrift-0.7.0/contrib/fb303/cpp/ServiceTracker.o

ALLSOURCES = $(SOURCES) $(YAMLSOURCES) $(MISCSOURCES)

$(BINDIR)/omni.server: $(ALLSOURCES)
	$(ECHO) "[CXX] linking bin/omni.server"
	$(MKDIR) -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $(BINDIR)/omni.server $(ALLSOURCES) $(LIBS)

all: $(BINDIR)/omni.server

ALLDEPS = $(shell find src | grep -Z --color=never ".cpp$$" | sed 's/\.cpp/\.d/g' | sed 's/src\//\.\/bin\//g' )

-include $(ALLDEPS)