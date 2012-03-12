# -*- Makefile -*-

HERE    	=       .
BINDIR		=	./bin
GENDIR	=	./build

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

CC      =	$(AT)gcc
CXX     =	$(AT)g++
THRIFT  =	$(AT)thrift
FPIC =	-fPIC

INCLUDES	=	-I$(HERE) \
				-I$(HERE)/src \
				-I$(HERE)/src/common \
				-I$(HERE)/src/server \
				-I$(HERE)/src/filesystem \
				-I$(HERE)/src/desktop \
				-I$(HERE)/src/thrift \
				-I$(HERE)/include \
				-I$(HERE)/zi_lib \
				-Iinclude/yaml-cpp/include \
				-Iinclude/libb64/include \
				-I/usr/include/thrift

LIBS = -lboost_filesystem \
	   -lboost_iostreams \
	   -lboost_system \
	   -lboost_thread \
	   -lboost_regex \
	   -lthrift \
	   -lthriftnb \
	   -lturbojpeg \
	   -lpng \
	   -lz \
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
						   $(FPIC) $(CXXWARN)

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
$(GENDIR)/%.d: src/%.cpp
	$(MKDIR) -p $(dir $@)
	$(CXX) $(CPP_DEPFLAGS) -MF $@ $<

# c++
$(GENDIR)/%.o: src/%.cpp
	$(ECHO) "[CXX] compiling $<"
	$(MKDIR) -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	$(MV) -f "$(@:.o=.T)" "$(@:.o=.d)"

# c
$(GENDIR)/%.o: src/%.c
	$(ECHO) "[CC] compiling $<"
	$(MKDIR) -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $<

# Thrift
.PHONY: thrift
thrift: if/server.thrift if/filesystem.thrift
	$(ECHO) "[Thrift] Generating..."
	$(MKDIR) -p src/thrift
	$(THRIFT) -r --out src/thrift --gen cpp if/server.thrift
	$(THRIFT) -r --out src/thrift --gen cpp if/filesystem.thrift
	$(RM) src/thrift/*.skeleton.cpp

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

COMMONSOURCES  = $(shell find src/common | grep -Z --color=never ".cpp$$" | sed 's/\.cpp/\.o/g' | sed 's/src\//\.\/build\//g' )
SERVERSOURCES  = $(shell find src/server | grep -Z --color=never ".cpp$$" | sed 's/\.cpp/\.o/g' | sed 's/src\//\.\/build\//g' )
FSSOURCES      = $(shell find src/filesystem | grep -Z --color=never ".cpp$$" | sed 's/\.cpp/\.o/g' | sed 's/src\//\.\/build\//g' )
DESKTOPSOURCES = $(shell find src/desktop | grep -Z --color=never ".cpp$$" | sed 's/\.cpp/\.o/g' | sed 's/src\//\.\/build\//g' )
THRIFTSOURCES  = $(shell find src/thrift | grep -Z --color=never ".cpp$$" | sed 's/\.cpp/\.o/g' | sed 's/src\//\.\/build\//g' )

YAMLSOURCES = $(shell find include/yaml-cpp/src | grep -Z --color=never ".cpp$$" | sed 's/\.cpp/\.o/g' )
MISCSOURCES = include/libb64/src/cencode.o \
			  external/srcs/thrift-0.7.0/contrib/fb303/cpp/FacebookBase.o \
			  external/srcs/thrift-0.7.0/contrib/fb303/cpp/ServiceTracker.o

SERVER = $(COMMONSOURCES) $(SERVERSOURCES) $(YAMLSOURCES) $(MISCSOURCES) $(THRIFTSOURCES)

$(BINDIR)/omni.server: thrift $(SERVER)
	$(ECHO) "[CXX] linking bin/omni.server"
	$(MKDIR) -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $(BINDIR)/omni.server $(SERVER) $(LIBS)

all: $(BINDIR)/omni.server

ALLDEPS = $(shell find src | grep -Z --color=never ".cpp$$" | sed 's/\.cpp/\.d/g' | sed 's/src\//\.\/build\//g' )

-include $(ALLDEPS)