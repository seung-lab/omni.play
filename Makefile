# -*- Makefile -*-

ifeq ($(shell uname), Darwin)
OSX = true
endif

HERE        =       .
EXTERNAL    =   $(HERE)/external/libs
BREAKPAD    =   $(HERE)/external/srcs/google-breakpad/src
GMOCK       =   $(HERE)/common/include/gmock-1.6.0
BASE64      =   $(HERE)/common/include/libb64
GENDIR      =   thrift/src

AT      =   @

CP      =   $(AT)cp
ECHO    =   $(AT)echo
MKDIR   =   $(AT)mkdir
MV      =   $(AT)mv
RM      =   $(AT)rm
TOUCH   =   $(AT)touch
TAR     =   $(AT)tar
FIND     =  $(AT)find
CC       =  $(AT)gcc
CXX      =  $(AT)g++

ifdef OSX
INT     =   $(AT)install_name_tool
endif

ifneq (,$(wildcard $(EXTERNAL)/thrift/bin/thrift))
THRIFT   =  $(AT)$(EXTERNAL)/thrift/bin/thrift
else
THRIFT   =  $(AT)thrift
endif

ifneq (,$(wildcard $(EXTERNAL)/qt/bin/moc))
  LOCAL_QT =  true
  MOC      =  $(AT)$(EXTERNAL)/qt/bin/moc
  RCC      =  $(AT)$(EXTERNAL)/qt/bin/rcc
else
  MOC      =  $(AT)moc
  RCC      =  $(AT)rcc
endif

ifneq (,$(wildcard $(EXTERNAL)/boost/lib/libboost_log.a))
  LOCAL_BOOST = true
endif

DUMPSYMS =  $(AT)$(EXTERNAL)/breakpad/bin/dump_syms

# don't delete intermediate files
.SECONDARY:

# Compile Flags ############################
CWARN       =   -Wall -Wno-sign-compare -Wunused-variable -Wreturn-type -Wno-unused-local-typedefs
CXXWARN     =   $(CWARN) -Wno-deprecated -Woverloaded-virtual -Wunused-but-set-variable -Wno-switch -Wno-unused-value -Wno-comment -Winvalid-pch

COMMON_CFLAGS       =   -g -std=gnu99 -D_GNU_SOURCE=1 \
				-D_REENTRANT -fPIC $(CWARN)

THRIFT_CXXFLAGS    =    -DHAVE_CONFIG_H

COMMON_CXXFLAGS    =    -g -fPIC $(CXXWARN) $(THRIFT_CXXFLAGS) -std=c++11 -MMD -MP -MT "$(@)" -MF $(@:.o=.d)

DBG_CFLAGS         =    $(COMMON_CFLAGS) -DDEBUG_MODE=1 -c -Wall -O0 --coverage
DBG_CXXFLAGS       =    $(COMMON_CXXFLAGS) -DDEBUG_MODE=1 -Og -gstabs+ --coverage
OPTIMIZATION_FLAGS =    -O2
OPT_CFLAGS         =    $(COMMON_CFLAGS) -DNDEBUG \
						$(OPTIMIZATION_FLAGS) -fno-omit-frame-pointer
OPT_CXXFLAGS       =    $(COMMON_CXXFLAGS) -DNDEBUG \
						$(OPTIMIZATION_FLAGS) -fno-omit-frame-pointer
COMMON_LDFLAGS     =    -g -fPIC -Wl,--eh-frame-hdr -lm
DBG_LDFLAGS        =    $(COMMON_LDFLAGS)
OPT_LDFLAGS        =    $(COMMON_LDFLAGS) -O2 -fno-omit-frame-pointer

ifdef LOCAL_BOOST
#-DBOOST_FILESYSTEM_NO_DEPRECATED -DBOOST_SYSTEM_NO_DEPRECATED
# Appears no longer necessary?
else
BOOST_DEFINES = -DBOOST_NO_CXX11_SCOPED_ENUMS -DBOOST_LOG_DYN_LINK
# BOOST_NO_CXX11_SCOPED_ENUMS is for linking w non-C++11 boost_filesystem
# BOOST_LOG_DYN_LINK is for dynamically linking to boost_log
endif

DEFINES = -DQT_NO_KEYWORDS -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_CORE_LIB \
-DQT_SHARED -DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS \
$(BOOST_DEFINES)

# comment out for clang until it supports openmp
EXTRA_CXXFLAGS = -DZI_USE_OPENMP -fopenmp
EXTRA_LDFLAGS  = -DZI_USE_OPENMP -fopenmp

ifneq ($(strip $(OPT)),)
  CFLAGS    =   $(OPT_CFLAGS) $(EXTRA_CFLAGS)
  CXXFLAGS  =   $(DEFINES) $(OPT_CXXFLAGS) $(EXTRA_CXXFLAGS)
  LDFLAGS   =   $(OPT_LDFLAGS) $(EXTRA_LDFLAGS)
  BUILDDIR  =   ./build/release
  BINDIR    =   ./bin/release
else
  CFLAGS    =   $(DBG_CFLAGS) $(EXTRA_CFLAGS)
  CXXFLAGS  =   $(DEFINES) $(DBG_CXXFLAGS) $(EXTRA_CXXFLAGS)
  LDFLAGS   =   $(DBG_LDFLAGS) $(EXTRA_LDFLAGS)
  BUILDDIR  =   ./build/debug
  BINDIR    =   ./bin/debug
endif

# Functions #####################################
define build_cpp
	$(ECHO) "[CXX] compiling $<"
	$(MKDIR) -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) $1 -o $@ $<
endef

define build_c
	$(ECHO) "[CC] compiling $<"
	$(MKDIR) -p $(dir $@)
	$(CC) -c $(CFLAGS) $1 -o $@ $<
endef

define build_gch
	$(ECHO) "[CXX] compiling $<"
	$(MKDIR) -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) $1 -o $@ $<
endef

define deps
	$(eval $1_SOURCES = $(shell find $2/src -iname "*.cpp" 2>/dev/null | grep -v "main.cpp"))
	$(eval $1_MAIN = $(BUILDDIR)/$2/main.o)
	$(eval $1_DEPS = $(subst $2/src,$(BUILDDIR)/$2, $($1_SOURCES:.cpp=.o)))
endef

define link
	$(ECHO) "[CXX] linking $@"
	$(MKDIR) -p $(dir $@)
	$(CXX) $(CXXFLAGS) -Wl,-rpath='$$ORIGIN' -o $@ $(filter-out %.mkcpp,$^) $1
endef

# Global Stuff #####################################
THRIFT_PROXY_DEPS = $(GENDIR)/server.thrift.mkcpp \
			        $(GENDIR)/rtm.thrift.mkcpp

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

LIB64_DEPS = $(BASE64)/src/cencode.o

TEST_DEPS = $(GMOCK)/src/gmock-all.o $(GMOCK)/gtest/src/gtest-all.o

ifdef OSX
.PHONY: all
all: app
else
.PHONY: all
all: common server desktop urtm valid export
endif

.PHONY: clean
clean:
	$(ECHO) Cleaning...
	$(RM) -rf bin build coverage
	$(RM) -rf common/src/precomp.h.gch desktop/src/precomp.h.gch
	$(RM) -f desktop/lib/strnatcmp.o
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
	cd $(HERE)/documentation/  && doxygen Doxyfile

# Common  #################################################
CURL_INCLUDES = $(shell pkg-config --cflags libcurl)
CURL_LIBS = $(shell pkg-config --libs libcurl)

INCLUDES    =   -I$(HERE) \
		-I$(HERE)/common/src \
		-I$(HERE)/common/include \
		-I$(HERE)/common/include/gmock-1.6.0/include \
		-I$(HERE)/common/include/gmock-1.6.0 \
		-I$(HERE)/common/include/gmock-1.6.0/gtest \
		-I$(HERE)/common/include/gmock-1.6.0/gtest/include \
		-I$(HERE)/zi_lib \
		-I$(BASE64)/include \
		$(CURL_INCLUDES)

#If we want to force the system to prefer static libs, we can use  
#BOOST_LIBS = -Wl,-Bstatic -l... -Wl,-Bdynamic
BOOST_LIBS = \
	   -lboost_filesystem \
	   -lboost_iostreams \
	   -lboost_log \
	   -lboost_system \
	   -lboost_thread \
	   -lboost_regex \
	   -lboost_date_time

LIBS = $(BOOST_LIBS) \
	   -L/usr/local/lib \
	   -lturbojpeg \
	   -lpthread -lrt -lGLU -lGL -lz \
	   $(CURL_LIBS) \
	   -L/usr/lib/libtcmalloc_minimal.a \
	   -lyaml-cpp  

COMMON_INCLUDES = $(INCLUDES) -include common/src/precomp.h

$(BUILDDIR)/common/%.o: common/src/%.cpp common/src/precomp.h.gch 
	$(call build_cpp, $(COMMON_INCLUDES))
common/src/precomp.h.gch: common/src/precomp.h
	$(call build_gch, $(INCLUDES))

$(eval $(call deps,COMMON,common))


COMMON_TEST_INCLUDES = -I$(HERE)/common/test/src

$(BUILDDIR)/common/test/%.o: common/test/src/%.cpp common/src/precomp.h.gch 
	$(call build_cpp, $(COMMON_INCLUDES) $(TEST_INCLUDES) $(COMMON_TEST_INCLUDES))

$(eval $(call deps,COMMON_TEST,common/test,$(COMMON_DEPS) $(TEST_DEPS)))

$(BINDIR)/omni.common.test: $(COMMON_TEST_DEPS) $(COMMON_TEST_MAIN)
	$(call link,$(LIBS))

.PHONY: common
common: $(BINDIR)/omni.common.test

# Thrift  #################################################
THRIFT_INCLUDES = $(COMMON_INCLUDES) \
				 -I$(GENDIR) \
				 -I/usr/local/include/thrift

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
# libpng in Ubuntu 14 apt repository is still version 1.2. 
# There doesn't seem to be any compatibility issue though
#			 -L$(EXTERNAL)/libpng/lib/ -lpng

$(BUILDDIR)/server/%.o: server/src/%.cpp $(THRIFT_PROXY_DEPS)
	$(call build_cpp, $(SERVER_INCLUDES))

$(eval $(call deps,SERVER,server,$(COMMON_DEPS) $(THRIFT_DEPS) $(LIB64_DEPS)))

$(BINDIR)/omni.server: $(SERVER_DEPS) $(SERVER_MAIN)
	$(call link,$(SERVERLIBS))


SERVER_TEST_INCLUDES = -I$(HERE)/server/test/src

$(BUILDDIR)/server/test/%.o: server/test/src/%.cpp $(THRIFT_PROXY_DEPS)
	$(call build_cpp, $(SERVER_INCLUDES) $(TEST_INCLUDES) $(SERVER_TEST_INCLUDES))

$(eval $(call deps,SERVER_TEST,server/test,$(SERVER_DEPS) $(TEST_DEPS)))

$(BINDIR)/omni.server.test: $(SERVER_TEST_DEPS) $(SERVER_TEST_MAIN)
	$(call link,$(SERVERLIBS))

.PHONY: server
server: common $(BINDIR)/omni.server $(BINDIR)/omni.server.test

# Desktop  #################################################
# QT_LIBRARIES = QtGui QtNetwork QtCore QtOpenGL
ifdef OSX
QT_INCLUDES = -I$(EXTERNAL)/qt/lib/Qt.framework/Headers \
				            -I$(EXTERNAL)/qt/lib/QtCore.framework/Headers \
				            -I$(EXTERNAL)/qt/lib/QtOpenGL.framework/Headers \
				            -I$(EXTERNAL)/qt/lib/QtGui.framework/Headers \
				            -I$(EXTERNAL)/qt/lib/QtNetwork.framework/Headers \

QT_LIBS = -F$(EXTERNAL)/qt/lib \
				       -framework QtCore \
				       -framework QtOpenGL \
				       -framework QtGui \
				       -framework QtNetwork \
				       -framework OpenGL \
				       -framework GLUT
else

  ifdef LOCAL_QT
    # Qt4.8 built from the bootstrap script
    QT_INCLUDES = -I$(EXTERNAL)/qt/include/Qt \
				            -I$(EXTERNAL)/qt/include/QtCore \
				            -I$(EXTERNAL)/qt/include/QtOpenGL \
				            -I$(EXTERNAL)/qt/include/QtGui \
				            -I$(EXTERNAL)/qt/include/QtNetwork
    QT_LIBS = -L$(EXTERNAL)/qt/lib \
					     -lQtGui \
					     -lQtNetwork \
					     -lQtCore \
					     -lQtOpenGL
  else
    #Qt5
    QT_INCLUDES = $(shell pkg-config --cflags \
    				Qt5Core Qt5OpenGL Qt5Gui Qt5Widgets Qt5Network 2>/dev/null)
    QT_LIBS = $(shell pkg-config --libs \
    				Qt5Core Qt5OpenGL Qt5Gui Qt5Widgets Qt5Network 2>/dev/null)

    #Qt4
    ifeq ($(QT_INCLUDES), )
      QT_INCLUDES = $(shell pkg-config --cflags \
      				QtCore QtOpenGL QtGui QtNetwork 2>/dev/null)
      QT_LIBS = $(shell pkg-config --libs \
      				QtCore QtOpenGL QtGui QtNetwork 2>/dev/null)
    endif
  endif

endif #OSX

HDF5_INCLUDES = -I$(EXTERNAL)/hdf5/include \
				-I/usr/lib/openmpi/include -I/usr/include/
HDF5_LIBS = -L$(EXTERNAL)/hdf5/lib/ -l hdf5
DESKTOP_INCLUDES = -I$(HERE)/desktop/src \
					$(INCLUDES) \
				  -I$(HERE)/desktop/include \
				  -I$(HERE)/desktop/lib \
				  -I$(HERE)/desktop \
				  $(HDF5_INCLUDES) \
				  -I$(BASE64)/include \
				  $(QT_INCLUDES) \
			    -include desktop/src/precomp.h

DESKTOPLIBS = $(LIBS) \
					$(HDF5_LIBS) \
					$(QT_LIBS)


$(BUILDDIR)/desktop/%.o: desktop/src/%.cpp desktop/src/precomp.h.gch
	$(call build_cpp, $(DESKTOP_INCLUDES))
desktop/src/precomp.h.gch: desktop/src/precomp.h
	$(call build_gch, $(DESKTOP_INCLUDES))

%.moc.cpp: %.hpp
	$(ECHO) "[MOC] Generating $<"
	$(MKDIR) -p $(dir $@)
	$(MOC) -o $@ $<
%.moc.cpp: %.h
	$(ECHO) "[MOC] Generating $<"
	$(MKDIR) -p $(dir $@)
	$(MOC) -o $@ $<
%.rcc.cpp: %.qrc
	$(ECHO) "[RCC] Generating $@"
	$(MKDIR) -p $(dir $@)
	$(RCC) -name $(basename $(notdir $<)) $< -o $@

DESKTOPHEADERS = $(shell grep Q_OBJECT -IR desktop/src | cut -f1 -d ':')
MOC_SRC0 = $(DESKTOPHEADERS:.hpp=.moc.cpp)
MOC_SRCS = $(MOC_SRC0:.h=.moc.cpp)
MOC_DEPS = $(subst desktop/src,$(BUILDDIR)/desktop,$(MOC_SRCS:.cpp=.o))

$(eval $(call deps,DESKTOP,desktop,$(COMMON_DEPS) $(MOC_DEPS) desktop/lib/strnatcmp.o $(BUILDDIR)/desktop/gui/resources.rcc.o))

$(BINDIR)/omni.desktop: $(DESKTOP_DEPS) $(DESKTOP_MAIN)
	$(call link,$(DESKTOPLIBS))

DESKTOP_TEST_INCLUDES = -I$(HERE)/desktop/test/src

$(BUILDDIR)/desktop/test/%.o: desktop/test/src/%.cpp desktop/src/precomp.h.gch
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

ifeq ($OSX, true)
APPDIR = $(BINDIR)/omni.desktop.app
APPBINDIR = $(APPDIR)/Contents/MacOS
APPFRAMEDIR = $(APPDIR)/Contents/Frameworks
APPRESCDIR = $(APPDIR)/Contents/Resources

QtMenuNib = external/srcs/qt-everywhere-opensource-src-4.8.2/src/gui/mac/qt_menu.nib

QtCore = $(abspath $(EXTERNAL)/qt/lib/QtCore.framework/Versions/4/QtCore)
QtGui = $(abspath $(EXTERNAL)/qt/lib/QtGui.framework/Versions/4/QtGui)
QtNetwork = $(abspath $(EXTERNAL)/qt/lib/QtNetwork.framework/Versions/4/QtNetwork)
QtOpenGL = $(abspath $(EXTERNAL)/qt/lib/QtOpenGL.framework/Versions/4/QtOpenGL)

.PHONY: app
app: $(BINDIR)/omni.desktop
	$(ECHO) "Assembling App..."
	$(MKDIR) -p $(APPBINDIR)
	$(MKDIR) -p $(APPFRAMEDIR)
	$(MKDIR) -p $(APPRESCDIR)

	$(CP) $(BINDIR)/omni.desktop $(APPBINDIR)

	$(CP) $(QtCore) $(APPFRAMEDIR)
	$(CP) $(QtGui) $(APPFRAMEDIR)
	$(CP) $(QtNetwork) $(APPFRAMEDIR)
	$(CP) $(QtOpenGL) $(APPFRAMEDIR)

	$(CP) -R $(QtMenuNib) $(APPRESCDIR)

	$(INT) -id @executable_path/../Frameworks/QtCore $(APPFRAMEDIR)/QtCore
	$(INT) -id @executable_path/../Frameworks/QtGui $(APPFRAMEDIR)/QtGui
	$(INT) -id @executable_path/../Frameworks/QtNetwork $(APPFRAMEDIR)/QtNetwork
	$(INT) -id @executable_path/../Frameworks/QtOpenGL $(APPFRAMEDIR)/QtOpenGL

	$(INT) -change $(QtCore) @executable_path/../Frameworks/QtCore $(APPFRAMEDIR)/QtGui
	$(INT) -change $(QtCore) @executable_path/../Frameworks/QtCore $(APPFRAMEDIR)/QtNetwork
	$(INT) -change $(QtCore) @executable_path/../Frameworks/QtCore $(APPFRAMEDIR)/QtOpenGL
	$(INT) -change $(QtGui) @executable_path/../Frameworks/QtGui $(APPFRAMEDIR)/QtOpenGL

	$(INT) -change $(QtCore) @executable_path/../Frameworks/QtCore $(APPBINDIR)/omni.desktop
	$(INT) -change $(QtGui) @executable_path/../Frameworks/QtGui $(APPBINDIR)/omni.desktop
	$(INT) -change $(QtNetwork) @executable_path/../Frameworks/QtNetwork $(APPBINDIR)/omni.desktop
	$(INT) -change $(QtOpenGL) @executable_path/../Frameworks/QtOpenGL $(APPBINDIR)/omni.desktop

	$(TAR) -zcvf $(BINDIR)/omni.tar.gz -C $(BINDIR) omni.desktop.app

endif

# URTM  #################################################
URTM_INCLUDES = $(SERVER_INCLUDES) \
				 -I$(HERE)/urtm/src

URTMLIBS = $(SERVERLIBS)

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

#BOOST_LIBS_EXPORT = -Wl,-Bstatic -lboost_program_options -Wl,-Bdynamic
BOOST_LIBS_EXPORT = -lboost_program_options 
EXPORTLIBS = $(LIBS) \
					$(BOOST_LIBS_EXPORT)

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


################################ Check code coverage of tests
.PHONY: coverage
coverage: 
	mkdir -p ./coverage
	#lcov --directory ./build/debug/common/ --directory ./build/debug/desktop/ --capture --output-file ./coverage/app.info
	lcov --directory ./build/debug/common/ --capture --output-file ./coverage/app.info
	lcov --remove ./coverage/app.info "/usr/include/*" -o ./coverage/app.info
	lcov --remove ./coverage/app.info "external/*" -o ./coverage/app.info  
	lcov --remove ./coverage/app.info "boost/*" -o ./coverage/app.info
	lcov --remove ./coverage/app.info "test/*" -o ./coverage/app.info   
	lcov --remove ./coverage/app.info "include/*" -o ./coverage/app.info   
	genhtml --output-directory ./coverage ./coverage/app.info    
