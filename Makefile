# -*- Makefile -*-

HERE        =       .
EXTERNAL    =   $(HERE)/external/libs
BREAKPAD    =   $(HERE)/external/srcs/google-breakpad/src
GMOCK       =   $(HERE)/common/include/gmock-1.6.0
BASE64      =   $(HERE)/common/include/libb64
# BINDIR        =   ./bin
# BUILDDIR  =   build
GENDIR      =   thrift/src

AT      =   @
DOLLAR  =   $$

CD      =   $(AT)cd
CP      =   $(AT)cp
ECHO    =   $(AT)echo
CAT     =   $(AT)cat
IF      =   $(AT)if
LN      =   $(AT)ln
MKDIR   =   $(AT)mkdir
MV      =   $(AT)mv
SED     =   $(AT)sed
RM      =   $(AT)rm
TOUCH   =   $(AT)touch
CHMOD   =   $(AT)chmod
DATE    =   $(AT)date
PERL    =   $(AT)perl
AR      =   $(AT)ar
TAR     =   $(AT)tar
FIND     =   $(AT)find
ARFLAGS =   rcs

CC       =  $(AT)gcc
CXX      =  $(AT)g++ -std=c++11
THRIFT   =  $(AT)$(EXTERNAL)/thrift/bin/thrift
MOC      =  $(AT)$(EXTERNAL)/qt/bin/moc
RCC      =  $(AT)$(EXTERNAL)/qt/bin/rcc
DUMPSYMS =  $(AT)$(EXTERNAL)/breakpad/bin/dump_syms
FPIC     =  -fPIC

# don't delete intermediate files
.SECONDARY:

# Compile Flags ############################
CWARN       =   -Wall -Wno-sign-compare -Wunused-variable -Wreturn-type -Wno-unused-local-typedefs
CXXWARN     =   $(CWARN) -Wno-deprecated -Woverloaded-virtual -Wunused-but-set-variable -Wno-switch -Wno-unused-value

CPP_DEPFLAGS        =   --std=c++11 -MM -MG -MP -MT "$(@:.d=.o)"
CPP_INLINE_DEPFLAGS =   -MMD -MP -MT "$(@)" -MF $(@:.o=.T)
COMMON_CFLAGS       =   -g -std=gnu99 -D_GNU_SOURCE=1 \
				-D_REENTRANT $(CPP_INLINE_DEPFLAGS) \
				$(FPIC) $(CWARN)

THRIFT_CXXFLAGS    =    -DHAVE_CONFIG_H

COMMON_CXXFLAGS    =    -g $(CPP_INLINE_DEPFLAGS) \
						   $(FPIC) $(CXXWARN) $(THRIFT_CXXFLAGS) -std=c++11

DBG_CFLAGS         =    $(COMMON_CFLAGS) -DDEBUG_MODE=1
DBG_CXXFLAGS       =    $(COMMON_CXXFLAGS) -DDEBUG_MODE=1 -gstabs+
OPTIMIZATION_FLAGS =    -O2
OPT_CFLAGS         =    $(COMMON_CFLAGS) -DNDEBUG \
						$(OPTIMIZATION_FLAGS) -fno-omit-frame-pointer
OPT_CXXFLAGS       =    $(COMMON_CXXFLAGS) -DNDEBUG \
						$(OPTIMIZATION_FLAGS) -fno-omit-frame-pointer
COMMON_LDFLAGS     =    -g $(FPIC) -Wl,--eh-frame-hdr -lm
DBG_LDFLAGS        =    $(COMMON_LDFLAGS) -gstabs+
OPT_LDFLAGS        =    $(COMMON_LDFLAGS) -O2 -fno-omit-frame-pointer

COMM_FLEX_FLAGS    =    -d
OPT_FLEXFLAGS      =    $(COMM_FLEX_FLAGS)
DBG_FLEXFLAGS      =    $(COMM_FLEX_FLAGS) -t

DEFINES = -DQT_NO_KEYWORDS -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_CORE_LIB \
-DQT_SHARED -DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS \
-DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED \
-DBOOST_MULTI_INDEX_DISABLE_SERIALIZATION \
-DBOOST_FILESYSTEM_NO_DEPRECATED -DBOOST_FILESYSTEM_VERSION=3 \
-DBOOST_SYSTEM_NO_DEPRECATED -DBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION \
-DBOOST_SPIRIT_USE_PHOENIX_V3

# comment out for clang until it supports openmp
EXTRA_CXXFLAGS = -DZI_USE_OPENMP -fopenmp
EXTRA_LDFLAGS  = -DZI_USE_OPENMP -fopenmp

ifneq ($(strip $(OPT)),)
  CFLAGS    =   $(OPT_CFLAGS) $(EXTRA_CFLAGS)
  CXXFLAGS  =   $(OPT_CXXFLAGS) $(EXTRA_CXXFLAGS)
  LDFLAGS   =   $(OPT_LDFLAGS) $(EXTRA_LDFLAGS)
  BUILDDIR  =   ./build/release
  BINDIR    =   ./bin/release
else
  CFLAGS    =   $(DBG_CFLAGS) $(EXTRA_CFLAGS)
  CXXFLAGS  =   $(DBG_CXXFLAGS) $(EXTRA_CXXFLAGS)
  LDFLAGS   =   $(DBG_LDFLAGS) $(EXTRA_LDFLAGS)
  BUILDDIR  =   ./build/debug
  BINDIR    =   ./bin/debug
endif

# Functions #####################################
define build_cpp
	$(ECHO) "[CXX] compiling $<"
	$(MKDIR) -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) -DBOOST_SPIRIT_USE_PHOENIX_V3 $1 -o $@ $<
	$(MV) -f "$(@:.o=.T)" "$(@:.o=.d)"
endef

define build_c
	$(ECHO) "[CC] compiling $<"
	$(MKDIR) -p $(dir $@)
	$(CC) -c $(CFLAGS) $1 -o $@ $<
endef

define make_d
	$(MKDIR) -p $(dir $@)
	$(CXX) $(CPP_DEPFLAGS) -DBOOST_SPIRIT_USE_PHOENIX_V3 $1 -MF $@ $<
endef

define deps
	$(eval $1_SOURCES = $(shell find $2/src -iname "*.cpp" 2>/dev/null | grep -v "main.cpp"))
	$(eval $1_MAIN = $(BUILDDIR)/$2/main.o)
	$(eval $1_DEPS = $(subst $2/src,$(BUILDDIR)/$2,$3 $($1_SOURCES:.cpp=.o)))
endef

define link
	$(ECHO) "[CXX] linking $@"
	$(MKDIR) -p $(dir $@)
	$(CXX) $(CXXFLAGS) -Wl,-rpath='$$ORIGIN' -o $@ $(filter-out %.mkcpp,$^) $1
endef

# Global Stuff #####################################
THRIFT_PROXY_DEPS = $(GENDIR)/server.thrift.mkcpp \
			        $(GENDIR)/rtm.thrift.mkcpp

%.d: %.cpp
	$(call make_d, $(INCLUDES))
%.o: %.cpp
	$(call build_cpp, $(INCLUDES))
%.o: %.cc
	$(call build_cpp, $(INCLUDES))
%.o: %.c
	$(call build_c, $(INCLUDES))

TEST_INCLUDES = -I$(GMOCK)/include \
			   -I$(GMOCK)/gtest/include \
			   -I$(GMOCK) \
			   -I$(GMOCK)/gtest \

YAML_SOURCES = $(shell find common/include/yaml-cpp/src -iname "*.cpp" )
YAML_DEPS = $(YAML_SOURCES:.cpp=.o)

LIB64_DEPS = $(BASE64)/src/cencode.o

TEST_DEPS = $(GMOCK)/src/gmock-all.o $(GMOCK)/gtest/src/gtest-all.o

.PHONY: all
all: common server desktop urtm valid export

.PHONY: clean
clean:
	$(ECHO) Cleaning...
	$(RM) -rf bin $(GENDIR) build
	$(RM) -f common/include/yaml-cpp/src/*.o common/include/yaml-cpp/src/*.d desktop/lib/strnatcmp.o
	$(FIND) desktop/src -iname "*\.moc\.cpp" -delete 2> /dev/null
	$(FIND) common/include -iname "*\.o" -delete 2> /dev/null

.PHONY: remake
remake: clean all

.PHONY: lint
lint:
	find desktop/src -iname "*.[hc]*" | grep -v "moc.cpp" | grep -v "rcc.cpp" | xargs \
		external/scripts/cpplint.py --filter=-legal/copyright

.PHONY: docs
docs:
	doxygen Doxyfile

# Common  #################################################
INCLUDES    =   -I$(HERE) \
		-I$(HERE)/common/src \
		-I$(HERE)/common/include \
		-I$(HERE)/common/include/gmock-1.6.0/include \
		-I$(HERE)/common/include/gmock-1.6.0 \
		-I$(HERE)/common/include/gmock-1.6.0/gtest \
		-I$(HERE)/common/include/gmock-1.6.0/gtest/include \
		-I$(HERE)/common/include/yaml-cpp/include \
		-I$(EXTERNAL)/libjpeg/include \
		-I$(HERE)/zi_lib \
		-I$(BASE64)/include \
		-I$(EXTERNAL)/boost/include \
		-I$(EXTERNAL)/netlib/include

LIBS = $(EXTERNAL)/boost/lib/libboost_filesystem.a \
	   $(EXTERNAL)/boost/lib/libboost_iostreams.a \
	   $(EXTERNAL)/boost/lib/libboost_log.a \
	   $(EXTERNAL)/boost/lib/libboost_system.a \
	   $(EXTERNAL)/boost/lib/libboost_thread.a \
	   $(EXTERNAL)/boost/lib/libboost_regex.a \
	   $(EXTERNAL)/boost/lib/libboost_date_time.a \
	   $(EXTERNAL)/libjpeg/lib/libturbojpeg.a \
	   -lpthread -lrt -lGLU -lGL -lz

$(BUILDDIR)/common/%.d: common/src/%.cpp
	$(call make_d, $(INCLUDES))
$(BUILDDIR)/common/%.o: common/src/%.cpp
	$(call build_cpp, $(INCLUDES))

$(eval $(call deps,COMMON,common,$(YAML_DEPS)))


COMMON_TEST_INCLUDES = -I$(HERE)/common/test/src

$(BUILDDIR)/common/test/%.d: common/test/src/%.cpp
	$(call make_d, $(INCLUDES) $(TEST_INCLUDES) $(COMMON_TEST_INCLUDES))
$(BUILDDIR)/common/test/%.o: common/test/src/%.cpp
	$(call build_cpp, $(INCLUDES) $(TEST_INCLUDES) $(COMMON_TEST_INCLUDES))

$(eval $(call deps,COMMON_TEST,common/test,$(COMMON_DEPS) $(TEST_DEPS)))

$(BINDIR)/omni.common.test: $(COMMON_TEST_DEPS) $(COMMON_TEST_MAIN)
	$(call link,$(LIBS))

.PHONY: common
common: $(BINDIR)/omni.common.test

# Thrift  #################################################
THRIFT_INCLUDES = $(INCLUDES) \
				 -I$(GENDIR) \
				 -I$(EXTERNAL)/thrift/include \
				 -I$(EXTERNAL)/thrift/include/thrift \

$(BUILDDIR)/thrift/%.d: $(GENDIR)/%.cpp $(THRIFT_DEPS)
	$(call make_d, $(THRIFT_INCLUDES))
$(BUILDDIR)/thrift/%.o: $(GENDIR)/%.cpp $(THRIFT_DEPS)
	$(call build_cpp, $(THRIFT_INCLUDES))

$(GENDIR)/%.thrift.mkcpp: thrift/if/%.thrift
	$(ECHO) "[Thrift] Generating $@"
	$(MKDIR) -p $(dir $@)
	$(TOUCH) $@.tmp
	$(THRIFT) -r --out $(GENDIR) --gen cpp $<
	$(RM) $(GENDIR)/*.skeleton.cpp
	$(MV) $@.tmp $@

$(eval $(call deps,THRIFT,thrift))

# Server  #################################################
SERVER_INCLUDES = $(THRIFT_INCLUDES) \
				 -I$(HERE)/server/src \
				 -I$(EXTERNAL)/libpng/include \

SERVERLIBS = $(EXTERNAL)/thrift/lib/libthrift.a \
			 $(EXTERNAL)/thrift/lib/libthriftnb.a \
			 $(EXTERNAL)/libpng/lib/libpng.a \
			 $(LIBS) \
			 -levent -lz

$(BUILDDIR)/server/%.d: server/src/%.cpp $(THRIFT_PROXY_DEPS)
	$(call make_d, $(SERVER_INCLUDES))
$(BUILDDIR)/server/%.o: server/src/%.cpp $(THRIFT_PROXY_DEPS)
	$(call build_cpp, $(SERVER_INCLUDES))

$(eval $(call deps,SERVER,server,$(COMMON_DEPS) $(THRIFT_DEPS) $(LIB64_DEPS)))

$(BINDIR)/omni.server: $(SERVER_DEPS) $(SERVER_MAIN)
	$(call link,$(SERVERLIBS))


SERVER_TEST_INCLUDES = -I$(HERE)/server/test/src

$(BUILDDIR)/server/test/%.d: server/test/src/%.cpp $(THRIFT_PROXY_DEPS)
	$(call make_d, $(SERVER_INCLUDES) $(TEST_INCLUDES) $(SERVER_TEST_INCLUDES))
$(BUILDDIR)/server/test/%.o: server/test/src/%.cpp $(THRIFT_PROXY_DEPS)
	$(call build_cpp, $(SERVER_INCLUDES) $(TEST_INCLUDES) $(SERVER_TEST_INCLUDES))

$(eval $(call deps,SERVER_TEST,server/test,$(SERVER_DEPS) $(TEST_DEPS)))

$(BINDIR)/omni.server.test: $(SERVER_TEST_DEPS) $(SERVER_TEST_MAIN)
	$(call link,$(SERVERLIBS))

.PHONY: server
server: common $(BINDIR)/omni.server $(BINDIR)/omni.server.test

# Desktop  #################################################
DESKTOP_INCLUDES = $(INCLUDES) \
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
				  -isystem$(EXTERNAL)/hdf5/include \
				  -I$(BASE64)/include \
				  -I$(BREAKPAD)

DESKTOPLIBS = $(LIBS) \
			  $(EXTERNAL)/hdf5/lib/libhdf5.a \
			  -L$(EXTERNAL)/qt/lib \
			  -lQtGui \
			  -lQtNetwork \
			  -lQtCore \
			  -lQtOpenGL

$(BUILDDIR)/desktop/%.d: desktop/src/%.cpp
	$(call make_d, $(DESKTOP_INCLUDES))
$(BUILDDIR)/desktop/%.o: desktop/src/%.cpp
	$(call build_cpp, $(DESKTOP_INCLUDES))

%.moc.cpp: %.hpp
	$(ECHO) "[MOC] Generating $<"
	$(MKDIR) -p $(dir $@)
	$(MOC) $(DEFINES) $(DESKTOP_INCLUDES) -o $@ $<
%.moc.cpp: %.h
	$(ECHO) "[MOC] Generating $<"
	$(MKDIR) -p $(dir $@)
	$(MOC) $(DEFINES) $(DESKTOP_INCLUDES) -o $@ $<
%.rcc.cpp: %.qrc
	$(ECHO) "[RCC] Generating $@"
	$(MKDIR) -p $(dir $@)
	$(RCC) -name $(basename $(notdir $<)) $< -o $@

DESKTOPHEADERS = $(shell grep Q_OBJECT -R desktop/src | cut -f1 -d ':')
MOC_SRC0 = $(DESKTOPHEADERS:.hpp=.moc.cpp)
MOC_SRCS = $(MOC_SRC0:.h=.moc.cpp)
MOC_DEPS = $(subst desktop/src,$(BUILDDIR)/desktop,$(MOC_SRCS:.cpp=.o))

$(eval $(call deps,DESKTOP,desktop,$(COMMON_DEPS) $(MOC_DEPS) desktop/lib/strnatcmp.o $(BUILDDIR)/desktop/gui/resources.rcc.o))

$(BINDIR)/omni.desktop: $(DESKTOP_DEPS) $(DESKTOP_MAIN)
	$(call link,$(DESKTOPLIBS))

DESKTOP_TEST_INCLUDES = -I$(HERE)/desktop/test/src

$(BUILDDIR)/desktop/test/%.d: desktop/test/src/%.cpp
	$(call make_d, $(DESKTOP_INCLUDES) $(TEST_INCLUDES) $(COMMON_TEST_INCLUDES) $(DESKTOP_TEST_INCLUDES))
$(BUILDDIR)/desktop/test/%.o: desktop/test/src/%.cpp
	$(call build_cpp, $(DESKTOP_INCLUDES) $(TEST_INCLUDES) $(COMMON_TEST_INCLUDES) $(DESKTOP_TEST_INCLUDES))

$(eval $(call deps,DESKTOP_TEST,desktop/test,$(DESKTOP_DEPS) $(TEST_DEPS)))

$(BINDIR)/omni.desktop.test: $(DESKTOP_TEST_DEPS) $(DESKTOP_TEST_MAIN)
	$(call link,$(DESKTOPLIBS))

$(BINDIR)/omni.tar.gz: desktop
	$(TAR) -zcvf $@ -C $(BINDIR) omni.desktop

omni.desktop.sym: $(BINDIR)/omni.desktop
	$(ECHO) [Breakpad] Generating Symbols...
	$(DUMPSYMS) $< > $@

.PHONY: symbols
symbols: omni.desktop.sym
	$(eval SYMS_HASH = $(shell head -n1 $< | cut -d' ' -f4))
	$(MKDIR) -p symbols/omni.desktop/$(SYMS_HASH)
	$(MV) $< symbols/omni.desktop/$(SYMS_HASH)

.PHONY: desktop

desktop: common $(BINDIR)/omni.desktop $(BINDIR)/omni.desktop.test

# URTM  #################################################
URTM_INCLUDES = $(SERVER_INCLUDES) \
				 -I$(HERE)/urtm/src

URTMLIBS = $(SERVERLIBS)

$(BUILDDIR)/urtm/%.d: urtm/src/%.cpp $(THRIFT_PROXY_DEPS)
	$(call make_d, $(URTM_INCLUDES))
$(BUILDDIR)/urtm/%.o: urtm/src/%.cpp $(THRIFT_PROXY_DEPS)
	$(call build_cpp, $(URTM_INCLUDES))

$(eval $(call deps,URTM,urtm,$(COMMON_DEPS) $(THRIFT_DEPS) $(LIB64_DEPS) $(BUILDDIR)/server/handler/updateMesh.o))

$(BINDIR)/omni.urtm: $(URTM_DEPS) $(URTM_MAIN)
	$(call link,$(URTMLIBS))

.PHONY: urtm
urtm: common $(BINDIR)/omni.urtm

# VALID  #################################################
VALID_INCLUDES = $(INCLUDES) \
				 -I$(HERE)/valid/src

VALIDLIBS = $(LIBS)

$(BUILDDIR)/valid/%.d: valid/src/%.cpp
	$(call make_d, $(VALID_INCLUDES))
$(BUILDDIR)/valid/%.o: valid/src/%.cpp
	$(call build_cpp, $(VALID_INCLUDES))

$(eval $(call deps,VALID,valid,$(COMMON_DEPS)))

$(BINDIR)/omni.valid: $(VALID_DEPS) $(VALID_MAIN)
	$(call link,$(VALIDLIBS))

.PHONY: valid
valid: common $(BINDIR)/omni.valid

# EXPORT  #################################################
EXPORT_INCLUDES = $(INCLUDES) \
				 -I$(HERE)/export/src

EXPORTLIBS = $(LIBS)

$(BUILDDIR)/export/%.d: export/src/%.cpp
	$(call make_d, $(EXPORT_INCLUDES))
$(BUILDDIR)/export/%.o: export/src/%.cpp
	$(call build_cpp, $(EXPORT_INCLUDES))

$(eval $(call deps,EXPORT,export,$(COMMON_DEPS)))

$(BINDIR)/omni.export: $(EXPORT_DEPS) $(EXPORT_MAIN)
	$(call link,$(EXPORTLIBS))

.PHONY: export
export: common $(BINDIR)/omni.export

# clang-format
ALL_SOURCE = $(shell find common/src common/test/src server/src server/test/src desktop/src desktop/test/src urtm/src valid/src -regex ".*\.[ch]\(pp\)?")
.PHONY: clang-format
clang-format:
	$(AT)clang-format -i --style=Google $(ALL_SOURCE)

# Automatic dependencies ####################################
ALLDEPS = $(shell find $(BUILDDIR) -iname "*.d" 2>/dev/null)
-include $(ALLDEPS)
