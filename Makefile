# -*- Makefile -*-

HERE    	=       .
EXTERNAL	=	$(HERE)/external/libs
BREAKPAD    =   $(HERE)/external/srcs/google-breakpad/src
GMOCK    	=   $(HERE)/common/include/gmock-1.6.0
# BINDIR		=	./bin
# BUILDDIR	=	build
GENDIR		=	thrift/src

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
TAR     =	$(AT)tar
ARFLAGS =	rcs

CC       =	$(AT)gcc
CXX      =	$(AT)g++
THRIFT   = 	$(AT)$(EXTERNAL)/thrift/bin/thrift
MOC	     =	$(AT)$(EXTERNAL)/qt/bin/moc
RCC	     =	$(AT)$(EXTERNAL)/qt/bin/rcc
DUMPSYMS =  $(AT)$(EXTERNAL)/breakpad/bin/dump_syms
FPIC     =	-fPIC

# Includes  #################################################
INCLUDES	=	-I$(HERE) \
				-I$(HERE)/common/src \
				-I$(HERE)/common/include \
				-I$(HERE)/common/include/yaml-cpp/include \
				-I$(HERE)/zi_lib \
				-I$(EXTERNAL)/boost/include

THRIFTINCLUDES = $(INCLUDES) \
				 -I$(HERE)/thrift/src \
				 -I$(EXTERNAL)/thrift/include/thrift \

SERVERINCLUDES = $(THRIFTINCLUDES) \
				 -I$(HERE)/server/src \
				 -I$(HERE)/filesystem/src \
				 -I$(EXTERNAL)/libjpeg/include \
				 -I$(EXTERNAL)/libpng/include \
				 -I$(HERE)/common/include/libb64/include \

DESKTOPINCLUDES = $(INCLUDES) \
				  -I$(HERE)/desktop/src \
				  -I$(HERE)/desktop/include \
				  -I$(HERE)/desktop/lib \
				  -I$(HERE)/desktop \
				  -I$(EXTERNAL)/qt/include/Qt \
				  -I$(EXTERNAL)/qt/include/QtCore \
				  -I$(EXTERNAL)/qt/include/QtOpenGL \
				  -I$(EXTERNAL)/qt/include/QtGui \
				  -I$(EXTERNAL)/qt/include/QtNetwork \
				  -I$(EXTERNAL)/qt/include \
				  -I$(EXTERNAL)/hdf5/include \
				  -I$(BREAKPAD)

TESTINCLUDES = -I$(GMOCK)/include \
 			   -I$(GMOCK)/gtest/include \
 			   -I$(GMOCK) \
 			   -I$(GMOCK)/gtest \

# Libs ##############################################
LIBS = $(EXTERNAL)/boost/lib/libboost_filesystem.a \
	   $(EXTERNAL)/boost/lib/libboost_iostreams.a \
	   $(EXTERNAL)/boost/lib/libboost_system.a \
	   $(EXTERNAL)/boost/lib/libboost_thread.a \
	   $(EXTERNAL)/boost/lib/libboost_regex.a \
	   -lpthread -lrt -lz

SERVERLIBS = $(LIBS) \
			 $(EXTERNAL)/thrift/lib/libthrift.a \
	   		 $(EXTERNAL)/thrift/lib/libthriftnb.a \
	   		 $(EXTERNAL)/libjpeg/lib/libturbojpeg.a \
	   		 $(EXTERNAL)/libpng/lib/libpng.a \
	   		 -levent


DESKTOPLIBS = $(LIBS) \
			  $(EXTERNAL)/hdf5/lib/libhdf5.a \
              $(EXTERNAL)/breakpad/lib/libbreakpad.a \
			  $(EXTERNAL)/breakpad/lib/libbreakpad_client.a \
			  -L$(EXTERNAL)/qt/lib \
              -lQtGui \
              -lQtNetwork \
              -lQtCore \
              -lQtOpenGL \
              -lGLU \
              -lGL

# Compile Flags ############################
CWARN		=	-Wall -Wno-sign-compare -Wno-unused-variable -Wno-return-type
CXXWARN		=	$(CWARN) -Wno-deprecated -Woverloaded-virtual -Wno-unused-but-set-variable -Wno-switch

CPP_DEPFLAGS		=	-MM -MG -MP -MT "$(@:.d=.o)"
CPP_INLINE_DEPFLAGS	=	-MMD -MP -MT "$(@)" -MF $(@:.o=.T)
COMMON_CFLAGS		=	-g -std=gnu99 -D_GNU_SOURCE=1 \
				-D_REENTRANT $(CPP_INLINE_DEPFLAGS) \
				$(FPIC) $(CWARN)

THRIFT_CXXFLAGS	   = 	-DHAVE_CONFIG_H

COMMON_CXXFLAGS    =	-g $(CPP_INLINE_DEPFLAGS) \
						   $(FPIC) $(CXXWARN) $(THRIFT_CXXFLAGS)

DBG_CFLAGS         =	$(COMMON_CFLAGS) -DDEBUG_MODE=1
DBG_CXXFLAGS       =	$(COMMON_CXXFLAGS) -DDEBUG_MODE=1 -gstabs
OPTIMIZATION_FLAGS =	-O3
OPT_CFLAGS         =	$(COMMON_CFLAGS) -DNDEBUG \
						$(OPTIMIZATION_FLAGS) -fno-omit-frame-pointer
OPT_CXXFLAGS       =	$(COMMON_CXXFLAGS) -DNDEBUG \
						$(OPTIMIZATION_FLAGS) -fno-omit-frame-pointer
COMMON_LDFLAGS     =	-g $(FPIC) -Wl,--eh-frame-hdr -lm
DBG_LDFLAGS        =	$(COMMON_LDFLAGS) -gstabs
OPT_LDFLAGS        =	$(COMMON_LDFLAGS) -O3 -fno-omit-frame-pointer

COMM_FLEX_FLAGS    =    -d
OPT_FLEXFLAGS      =    $(COMM_FLEX_FLAGS)
DBG_FLEXFLAGS      =    $(COMM_FLEX_FLAGS) -t

DEFINES = -DQT_NO_KEYWORDS -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB -DQT_SHARED \
-DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED -DBOOST_MULTI_INDEX_DISABLE_SERIALIZATION \
-DBOOST_FILESYSTEM_NO_DEPRECATED -DBOOST_FILESYSTEM_VERSION=3 -DBOOST_SYSTEM_NO_DEPRECATED

# comment out for clang until it supports openmp
EXTRA_CXXFLAGS = -DZI_USE_OPENMP -fopenmp
EXTRA_LDFLAGS  = -DZI_USE_OPENMP -fopenmp

ifneq ($(strip $(OPT)),)
  CFLAGS	=	$(OPT_CFLAGS) $(EXTRA_CFLAGS)
  CXXFLAGS	=	$(OPT_CXXFLAGS) $(EXTRA_CXXFLAGS)
  LDFLAGS	=	$(OPT_LDFLAGS) $(EXTRA_LDFLAGS)
  BUILDDIR	=	./build/release
  BINDIR	=	./bin/release
else
  CFLAGS	=	$(DBG_CFLAGS) $(EXTRA_CFLAGS)
  CXXFLAGS	=	$(DBG_CXXFLAGS) $(EXTRA_CXXFLAGS)
  LDFLAGS	=	$(DBG_LDFLAGS) $(EXTRA_LDFLAGS)
  BUILDDIR	=	./build/debug
  BINDIR	=	./bin/debug
endif

# Build functions #####################################
define build_cpp
	$(ECHO) "[CXX] compiling $<"
	$(MKDIR) -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) $1 -o $@ $<
	$(MV) -f "$(@:.o=.T)" "$(@:.o=.d)"
endef

define build_c
	$(ECHO) "[CC] compiling $<"
	$(MKDIR) -p $(dir $@)
	$(CC) -c $(CFLAGS) $1 -o $@ $<
endef

define make_d
	$(MKDIR) -p $(dir $@)
	$(CXX) $(CPP_DEPFLAGS) $1 -MF $@ $<
endef

THRIFT_DEPS = $(GENDIR)/server.thrift.mkcpp \
			  $(GENDIR)/rtm.thrift.mkcpp

# Build Directives ###################################
$(BUILDDIR)/common/%.d: common/src/%.cpp
	$(call make_d, $(INCLUDES))
$(BUILDDIR)/common/%.o: common/src/%.cpp
	$(call build_cpp, $(INCLUDES))

$(BUILDDIR)/common/test/%.d: common/test/src/%.cpp
	$(call make_d, $(INCLUDES) $(TESTINCLUDES))
$(BUILDDIR)/common/test/%.o: common/test/src/%.cpp
	$(call build_cpp, $(INCLUDES) $(TESTINCLUDES))

$(BUILDDIR)/thrift/%.d: thrift/src/%.cpp $(THRIFT_DEPS)
	$(call make_d, $(THRIFTINCLUDES))
$(BUILDDIR)/thrift/%.o: thrift/src/%.cpp $(THRIFT_DEPS)
	$(call build_cpp, $(THRIFTINCLUDES))

$(BUILDDIR)/server/%.d: server/src/%.cpp $(THRIFT_DEPS)
	$(call make_d, $(SERVERINCLUDES))
$(BUILDDIR)/server/%.o: server/src/%.cpp $(THRIFT_DEPS)
	$(call build_cpp, $(SERVERINCLUDES))

$(BUILDDIR)/server/test/%.d: server/test/src/%.cpp $(THRIFT_DEPS)
	$(call make_d, $(SERVERINCLUDES) $(TESTINCLUDES))
$(BUILDDIR)/server/test/%.o: server/test/src/%.cpp $(THRIFT_DEPS)
	$(call build_cpp, $(SERVERINCLUDES) $(TESTINCLUDES))

$(BUILDDIR)/desktop/%.d: desktop/src/%.cpp $(THRIFT_DEPS)
	$(call make_d, $(DESKTOPINCLUDES))
$(BUILDDIR)/desktop/%.o: desktop/src/%.cpp $(THRIFT_DEPS)
	$(call build_cpp, $(DESKTOPINCLUDES))

$(BUILDDIR)/desktop/test/%.d: desktop/test/src/%.cpp $(THRIFT_DEPS)
	$(call make_d, $(DESKTOPINCLUDES) $(TESTINCLUDES))
$(BUILDDIR)/desktop/test/%.o: desktop/test/src/%.cpp $(THRIFT_DEPS)
	$(call build_cpp, $(DESKTOPINCLUDES) $(TESTINCLUDES))

%.d: %.cpp
	$(call make_d, $(INCLUDES))
%.o: %.cpp
	$(call build_cpp, $(INCLUDES))
%.o: %.cc
	$(call build_cpp, $(INCLUDES))
%.o: %.c
	$(call build_c, $(INCLUDES))

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

$(GENDIR)/%.thrift.mkcpp: thrift/if/%.thrift
	$(ECHO) "[Thrift] Generating $@"
	$(MKDIR) -p $(dir $@)
	$(TOUCH) $@.tmp
	$(THRIFT) -r --out $(GENDIR) --gen cpp $<
	$(RM) $(GENDIR)/*.skeleton.cpp
	$(MV) $@.tmp $@

# Dependencies #################################
define deps
	$(eval $1_SOURCES = $(shell find $2/src -iname "*.cpp"  | grep -v "main.cpp"))
	$(eval $1_MAIN = $(BUILDDIR)/$2/main.o)
	$(eval $1_DEPS = $(subst $2/src,$(BUILDDIR)/$2,$($1_SOURCES:.cpp=.o)))
endef

$(eval $(call deps,COMMON,common))
$(eval $(call deps,COMMON_TEST,common/test))
$(eval $(call deps,SERVER,server))
$(eval $(call deps,SERVER_TEST,server/test))
$(eval $(call deps,DESKTOP,desktop))
$(eval $(call deps,DESKTOP_TEST,desktop/test))

YAML_SOURCES = $(shell find common/include/yaml-cpp/src -iname "*.cpp" )
YAML_DEPS = $(YAML_SOURCES:.cpp=.o)

LIB64_DEPS = common/include/libb64/src/cencode.o

TEST_DEPS = $(GMOCK)/src/gmock-all.o $(GMOCK)/gtest/src/gtest-all.o

DESKTOPHEADERS = $(subst desktop/src,$(BUILDDIR)/desktop, \
               	 	$(shell grep Q_OBJECT -R desktop/src | cut -f1 -d ':'))
MOC_SRCS := $($(DESKTOPHEADERS:.hpp=.moc.cpp):.h=.moc.cpp)
MOC_DEPS := $(MOC_SRCS:.cpp=.o)

define link
	$(ECHO) "[CXX] linking $@"
	$(MKDIR) -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $(filter-out %.mkcpp,$^) $1
endef

# Targets ####################################
.PHONY: all
all: common server desktop

$(BINDIR)/omni.common.test: $(COMMON_DEPS) \
							$(COMMON_TEST_DEPS) \
							$(YAML_DEPS) \
							$(TEST_DEPS) \
							$(COMMON_TEST_MAIN)
	$(call link,$(LIBS))
	$@

$(BINDIR)/omni.desktop: $(COMMON_DEPS)\
					    $(DESKTOP_DEPS)\
					    $(DESKTOP_MAIN)\
					    desktop/lib/strnatcmp.o\
					    $(BUILDDIR)/desktop/gui/resources.rcc.o
	$(call link,$(DESKTOPLIBS))

$(BINDIR)/omni.desktop.test: $(COMMON_DEPS)\
							 $(DESKTOP_DEPS)\
							 $(DESKTOP_TEST_DEPS)\
							 $(DESKTOP_TEST_MAIN)\
							 desktop/lib/strnatcmp.o\
							 $(BUILDDIR)/desktop/gui/resources.rcc.o
	$(call link,$(DESKTOPLIBS))
	$@

$(BINDIR)/omni.server: $(COMMON_DEPS)\
					   $(SERVER_DEPS)\
					   $(THRIFT_DEPS)\
					   $(SERVER_MAIN)
	$(link)

$(BINDIR)/omni.server.test:$(COMMON_DEPS)\
					   	   $(SERVER_DEPS)\
					   	   $(SERVER_TEST_DEPS)\
					   	   $(THRIFT_DEPS)\
					   	   $(SERVER_TEST_MAIN)
	$(link)
	$@

$(BINDIR)/omni.tar.gz: desktop
	$(TAR) -zcvf $@ -C $(BINDIR) omni.desktop

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
	$(RM) -rf bin $(GENDIR) build
	$(RM) common/include/yaml-cpp/src/*.o common/include/yaml-cpp/src/*.d

.PHONY: remake
remake: clean all

.PHONY: lint
lint:
	find desktop/src -iname "*.[hc]*" | grep -v "moc.cpp" | grep -v "rcc.cpp" | xargs \
		external/scripts/cpplint.py --filter=-legal/copyright

omni.desktop.sym: $(BINDIR)/omni.desktop
	$(ECHO) [Breakpad] Generating Symbols...
	$(DUMPSYMS) $< > $@

.PHONY: symbols
symbols: omni.desktop.sym
	$(eval SYMS_HASH = $(shell head -n1 $< | cut -d' ' -f4))
	$(MKDIR) -p symbols/omni.desktop/$(SYMS_HASH)
	$(MV) $< symbols/omni.desktop/$(SYMS_HASH)

.PHONY: common
common: $(BINDIR)/omni.common.test

.PHONY: desktop
desktop: common $(BINDIR)/omni.desktop $(BINDIR)/omni.desktop.test
	$(BINDIR)/omni.desktop.test

.PHONY: server
server: common $(BINDIR)/omni.server $(BINDIR)/omni.server.test
#	$(BINDIR)/omni.server.test

ALLDEPS = $(shell find $(BUILDDIR) -iname "*.d")
-include $(ALLDEPS)
