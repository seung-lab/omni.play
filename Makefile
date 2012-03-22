# -*- Makefile -*-

HERE    	=       .
EXTERNAL	=	$(HERE)/external/libs
BINDIR		=	./bin
BUILDDIR	=	build

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
FPIC   =	-fPIC

INCLUDES	=	-I$(HERE) \
				-I$(HERE)/common/src \
				-I$(HERE)/common/include \
				-I$(HERE)/common/include/yaml-cpp/include \
				-I$(HERE)/common/include/libb64/include \
				-I$(HERE)/server/src \
				-I$(HERE)/filesystem/src \
				-I$(HERE)/desktop/src \
				-I$(HERE)/zi_lib \
				-I$(EXTERNAL)/thrift/include/thrift \
				-I$(EXTERNAL)/Boost/include \
				-I$(EXTERNAL)/libjpeg/include \
				-I$(EXTERNAL)/libpng/include \


LIBS = $(EXTERNAL)/Boost/lib/libboost_filesystem.a \
	   $(EXTERNAL)/Boost/lib/libboost_iostreams.a \
	   $(EXTERNAL)/Boost/lib/libboost_system.a \
	   $(EXTERNAL)/Boost/lib/libboost_thread.a \
	   $(EXTERNAL)/Boost/lib/libboost_regex.a \
	   $(EXTERNAL)/thrift/lib/libthrift.a \
	   $(EXTERNAL)/thrift/lib/libthriftnb.a \
	   $(EXTERNAL)/libjpeg/lib/libturbojpeg.a \
	   $(EXTERNAL)/libpng/lib/libpng.a \
	   -levent -lpthread -levent -lrt 

CXX_INCLUDES	=	$(INCLUDES)

CWARN		=	-Wall -Wno-sign-compare -Wno-unused-variable -Wno-return-type
CXXWARN		=	$(CWARN) -Wno-deprecated -Woverloaded-virtual

CPP_DEPFLAGS		=	-MM -MG -MP $(CXX_INCLUDES) -MT "$(@:.d=.o)"
CPP_INLINE_DEPFLAGS	=	-MMD -MP -MT "$(@)" -MF $(@:.o=.T)
COMMON_CFLAGS		=	-g -std=gnu99 -D_GNU_SOURCE=1 \
				-D_REENTRANT $(CPP_INLINE_DEPFLAGS) \
				$(INCLUDES) $(FPIC) $(CWARN)

THRIFT_CXXFLAGS	   = 	-DHAVE_CONFIG_H

COMMON_CXXFLAGS    =	-g $(CPP_INLINE_DEPFLAGS) $(CXX_INCLUDES) \
						   $(FPIC) $(CXXWARN) $(THRIFT_CXXFLAGS)

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

VPATH = common/src:server/src:filesystem/src:desktop/src

# dependency files for c++
build/%.d: %.cpp
	$(MKDIR) -p $(dir $@)
	$(CXX) $(CPP_DEPFLAGS) -MF $@ $<

# c++
build/%.o: %.cpp
	$(ECHO) "[CXX] compiling $<"
	$(MKDIR) -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	$(MV) -f "$(@:.o=.T)" "$(@:.o=.d)"

# c
build/%.o: %.c
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

common/src/thrift/%.thrift.mkcpp: common/if/%.thrift
	$(ECHO) "[Thrift ] Generating $@"
	$(MKDIR) -p $(dir $@)
	$(TOUCH) $@.tmp
	$(THRIFT) -r --out common/src/thrift --gen cpp $<
	$(RM) common/src/thrift/*.skeleton.cpp
	$(MV) $@.tmp $@

.PHONY: all
all: $(BINDIR)/omni.server

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

COMMONSOURCES = $(subst common/src,build,$(shell find common/src -iname "*.cpp"))
SERVERSOURCES = $(subst server/src,build,$(shell find server/src -iname "*.cpp"))
DESKTOPSOURCES = $(shell find desktop/src -iname "*.cpp")
FILESYSTEMSOURCES = $(shell find filesystem/src -iname "*.cpp")

YAMLSOURCES = $(shell find common/include/yaml-cpp/src -iname "*.cpp" )
LIB64SOURCES = common/include/libb64/src/cencode.o

SERVER_SRCS = $(COMMONSOURCES) $(SERVERSOURCES) $(YAMLSOURCES) $(LIB64SOURCES)
SERVER_DEPS := $(SERVER_SRCS:.cpp=.o)

$(BINDIR)/omni.server: common/src/thrift/server.thrift.mkcpp $(SERVER_DEPS)
	$(ECHO) "[CXX] linking bin/omni.server"
	$(MKDIR) -p $(dir $@)
	$(CXX) $(CXXFLAGS) -static-libgcc -static-libstdc++ -o $(BINDIR)/omni.server $(SERVER_DEPS) $(LIBS)

ALLDEPS = $(shell find build -iname "*.d")

-include $(ALLDEPS)