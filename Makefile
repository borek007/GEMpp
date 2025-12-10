## GEM++ Makefile (Qt + Make, no qmake)
## Targets:
##   make            -> build everything (GLPK vendored)
##   make gempp      -> CLI app
##   make gempp-gui  -> GUI app
##   make glpk       -> download+build static GLPK (self contained)
##   make clean      -> remove build artifacts

# Qt detection (prefers Qt6, falls back to Qt5)
QT_BASE      := $(shell pkg-config --exists Qt6Core && echo Qt6 || echo Qt5)
QT_MODULES   := $(QT_BASE)Core $(QT_BASE)Gui $(QT_BASE)Widgets $(QT_BASE)Xml
QT_CFLAGS    := $(shell pkg-config --cflags $(QT_MODULES))
QT_LDFLAGS   := $(shell pkg-config --libs $(QT_MODULES))

# moc/rcc discovery
MOC ?= $(shell command -v moc-qt6 || command -v moc-qt5 || command -v moc)
RCC ?= $(shell command -v rcc-qt6 || command -v rcc-qt5 || command -v rcc)
ifeq ($(MOC),)
$(error moc not found; install Qt dev tools)
endif
ifeq ($(RCC),)
$(error rcc not found; install Qt dev tools)
endif

# Toolchain
CXX      ?= g++
AR       ?= ar
OS       := $(shell uname)
BUILD    := build
OBJDIR   := $(BUILD)/obj
LIBDIR   := $(BUILD)/lib
BINDIR   := $(BUILD)/bin
MOCDIR   := $(BUILD)/moc
RCCDIR   := $(BUILD)/rcc

ifeq ($(OS),Darwin)
SO_EXT       := dylib
SO_FLAG      := -dynamiclib
SONAME_FLAG  := -Wl,-install_name,@rpath
EXE_EXT      :=
else
SO_EXT       := so
SO_FLAG      := -shared
SONAME_FLAG  := -Wl,-soname
EXE_EXT      :=
endif

# GLPK vendoring (download + static build)
GLPK_VERSION ?= 4.65
GLPK_URL     := https://ftp.gnu.org/gnu/glpk/glpk-$(GLPK_VERSION).tar.gz
GLPK_SRC_DIR := third_party/glpk/src
GLPK_BUILD   := third_party/glpk/build
GLPK_PREFIX  := $(abspath $(GLPK_BUILD)/install)
GLPK_LIB     := $(GLPK_PREFIX)/lib/libglpk.a
GLPK_INCLUDE := $(GLPK_PREFIX)/include

# Core include/libs
INCLUDES := -Isrc -Isrc/library -Isrc/dependencies/QGar -I$(GLPK_INCLUDE)
CXXFLAGS ?= -std=c++17 -O2
CXXFLAGS += -Wall -Wextra -fPIC $(QT_CFLAGS) $(INCLUDES)
LDFLAGS  += $(QT_LDFLAGS)
ifneq ($(OS),Darwin)
LDFLAGS  += -ldl
endif

# Source discovery
LIB_SRCS    := $(shell find src/library -name '*.cpp')
QGAR_SRCS   := $(shell find src/dependencies/QGar/qgarlib -name '*.cpp')
CORE_SRCS   := $(shell find src/apps/Core -name '*.cpp')
GUI_SRCS    := $(shell find src/apps/GUI -name '*.cpp')
MOLECULE_SRCS := $(shell find src/apps/Molecule -name '*.cpp')
PLUGIN_GLPK_SRCS := $(shell find src/plugins/GLPK -name '*.cpp')

# Qt resource (GUI)
GUI_QRC     := src/apps/GUI/GUI.qrc
GUI_RCC_CPP := $(RCCDIR)/GUI_qrc.cpp
GUI_SRCS    += $(GUI_RCC_CPP)

# moc generation for headers containing Q_OBJECT
MOC_HEADERS         := $(shell grep -rl "Q_OBJECT" src/library src/apps src/plugins src/dependencies/QGar/qgarlib | grep -E '\.h$$')
MOC_LIB_HEADERS     := $(filter src/library/% src/dependencies/%,$(MOC_HEADERS))
MOC_PLUGIN_HEADERS  := $(filter src/plugins/%,$(MOC_HEADERS))
MOC_APP_HEADERS     := $(filter src/apps/%,$(MOC_HEADERS))

MOC_LIB_SRCS        := $(patsubst %.h,$(MOCDIR)/%.moc.cpp,$(MOC_LIB_HEADERS))
MOC_PLUGIN_SRCS     := $(patsubst %.h,$(MOCDIR)/%.moc.cpp,$(MOC_PLUGIN_HEADERS))
MOC_APP_SRCS        := $(patsubst %.h,$(MOCDIR)/%.moc.cpp,$(MOC_APP_HEADERS))

# Object mapping
LIB_OBJS         := $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(LIB_SRCS)) \
                    $(patsubst $(BUILD)/%.cpp,$(OBJDIR)/%.o,$(MOC_LIB_SRCS))
QGAR_OBJS        := $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(QGAR_SRCS))
CORE_OBJS        := $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(CORE_SRCS))
GUI_OBJS         := $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(GUI_SRCS)) \
                    $(patsubst $(BUILD)/%.cpp,$(OBJDIR)/%.o,$(MOC_APP_SRCS)) \
                    $(patsubst $(BUILD)/%.cpp,$(OBJDIR)/%.o,$(GUI_RCC_CPP))
MOLECULE_OBJS    := $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(MOLECULE_SRCS)) \
                    $(patsubst $(BUILD)/%.cpp,$(OBJDIR)/%.o,$(MOC_APP_SRCS))
PLUGIN_GLPK_OBJS := $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(PLUGIN_GLPK_SRCS)) \
                    $(patsubst $(BUILD)/%.cpp,$(OBJDIR)/%.o,$(MOC_PLUGIN_SRCS))

# Targets
LIB_QGAR   := $(LIBDIR)/libqgar.a
LIB_GEMPP  := $(LIBDIR)/libGEM++.${SO_EXT}
PLUGIN_GLPK:= $(LIBDIR)/libGEM++GLPK.${SO_EXT}
BIN_CORE   := $(BINDIR)/gempp$(EXE_EXT)
BIN_GUI    := $(BINDIR)/gempp-gui$(EXE_EXT)
BIN_MOLECULE := $(BINDIR)/gempp-molecule$(EXE_EXT)

.PHONY: all gempp gempp-gui glpk clean

all: glpk $(LIB_GEMPP) $(PLUGIN_GLPK) $(BIN_CORE) $(BIN_GUI) $(BIN_MOLECULE)

gempp: $(BIN_CORE)
gempp-gui: $(BIN_GUI)

# GLPK download/build
$(GLPK_LIB):
	@echo "==> Fetch/build GLPK $(GLPK_VERSION)"
	@mkdir -p $(GLPK_BUILD)
	@rm -rf $(GLPK_SRC_DIR)
	@mkdir -p $(dir $(GLPK_SRC_DIR))
	curl -L $(GLPK_URL) | tar xz -C $(dir $(GLPK_SRC_DIR))
	mv $(dir $(GLPK_SRC_DIR))/glpk-$(GLPK_VERSION) $(GLPK_SRC_DIR)
	cd $(GLPK_SRC_DIR) && ./configure --prefix=$(GLPK_PREFIX) --enable-shared=no --enable-static=yes
	$(MAKE) -C $(GLPK_SRC_DIR)
	$(MAKE) -C $(GLPK_SRC_DIR) install

glpk: $(GLPK_LIB)

# Libraries
$(LIBDIR)/libqgar.a: $(QGAR_OBJS) | $(LIBDIR)
	$(AR) rcs $@ $(QGAR_OBJS)

$(LIB_GEMPP): $(LIB_OBJS) $(LIB_QGAR) $(GLPK_LIB) | $(LIBDIR)
	$(CXX) $(SO_FLAG) $(SONAME_FLAG),$(notdir $(LIB_GEMPP)) -o $@ $(LIB_OBJS) -L$(LIBDIR) -lqgar $(GLPK_LIB) $(LDFLAGS)

$(PLUGIN_GLPK): $(PLUGIN_GLPK_OBJS) $(LIB_GEMPP) $(GLPK_LIB) | $(LIBDIR)
	$(CXX) $(SO_FLAG) $(SONAME_FLAG),$(notdir $(PLUGIN_GLPK)) -o $@ $(PLUGIN_GLPK_OBJS) -L$(LIBDIR) -lGEM++ $(GLPK_LIB) $(LDFLAGS)

# Applications
$(BIN_CORE): $(CORE_OBJS) $(LIB_GEMPP) | $(BINDIR)
	$(CXX) -o $@ $(CORE_OBJS) -L$(LIBDIR) -lGEM++ $(GLPK_LIB) $(LDFLAGS)

$(BIN_GUI): $(GUI_OBJS) $(LIB_GEMPP) | $(BINDIR)
	$(CXX) -o $@ $(GUI_OBJS) -L$(LIBDIR) -lGEM++ $(GLPK_LIB) $(LDFLAGS)

$(BIN_MOLECULE): $(MOLECULE_OBJS) $(LIB_GEMPP) | $(BINDIR)
	$(CXX) -o $@ $(MOLECULE_OBJS) -L$(LIBDIR) -lGEM++ $(GLPK_LIB) $(LDFLAGS)

# Object compilation
$(OBJDIR)/%.o: src/%.cpp | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(BUILD)/%.cpp | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# moc generation
$(MOCDIR)/%.moc.cpp: %.h | $(MOCDIR)
	@mkdir -p $(dir $@)
	$(MOC) -o $@ $<

# rcc generation
$(RCCDIR)/%_qrc.cpp: src/apps/GUI/%.qrc | $(RCCDIR)
	@mkdir -p $(dir $@)
	$(RCC) -o $@ $<

# Directories
$(LIBDIR) $(BINDIR) $(OBJDIR) $(MOCDIR) $(RCCDIR):
	@mkdir -p $@

clean:
	rm -rf $(BUILD) third_party/glpk/src third_party/glpk/build


