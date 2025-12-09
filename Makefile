# Cross-platform Makefile for GEM++
# Supports macOS, Linux, and Windows (via MinGW)

# Detect platform
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
    PLATFORM = macos
    CXX = clang++
    AR = ar
    SHARED_EXT = .dylib
    SHARED_FLAGS = -dynamiclib
    CXXFLAGS += -std=c++11 -stdlib=libc++
    LDFLAGS += -stdlib=libc++
else ifeq ($(UNAME), Linux)
    PLATFORM = linux
    CXX = g++
    AR = ar
    SHARED_EXT = .so
    SHARED_FLAGS = -shared
    CXXFLAGS += -std=c++11 -fPIC
else ifeq ($(OS), Windows_NT)
    PLATFORM = windows
    CXX = g++
    AR = ar
    SHARED_EXT = .dll
    SHARED_FLAGS = -shared
    CXXFLAGS += -std=c++11
else
    $(error Unsupported platform)
endif

# Build configuration
BUILD_TYPE ?= release
PREFIX ?= /usr/local

# Directories
SRC_DIR = src
BUILD_DIR = build/$(PLATFORM)/$(BUILD_TYPE)
BIN_DIR = $(BUILD_DIR)/bin
LIB_DIR = $(BUILD_DIR)/lib
OBJ_DIR = $(BUILD_DIR)/obj

# GLPK configuration
GLPK_DIR = ressources/glpk/src
GLPK_BUILD_DIR = $(BUILD_DIR)/glpk
GLPK_LIB = $(LIB_DIR)/libglpk.a

# Compiler flags
CXXFLAGS += -I$(SRC_DIR)/library -I$(GLPK_DIR)/src -Wall -Wextra
ifeq ($(BUILD_TYPE), debug)
    CXXFLAGS += -g -DDEBUG
else
    CXXFLAGS += -O2 -DNDEBUG
endif

# Targets
.PHONY: all clean install glpk library plugins apps

all: glpk library plugins apps

# GLPK static library
glpk: $(GLPK_LIB)

$(GLPK_LIB):
	@echo "Building GLPK..."
	@mkdir -p $(GLPK_BUILD_DIR)
	cd $(GLPK_BUILD_DIR) && ../../../../$(GLPK_DIR)/configure --disable-shared --enable-static \
		--prefix=$(PREFIX) --with-gmp=no && make -j$(shell nproc || echo 4)
	@mkdir -p $(LIB_DIR)
	cp $(GLPK_BUILD_DIR)/src/.libs/libglpk.a $(GLPK_LIB)

# Library sources (excluding Qt-dependent files)
LIB_SOURCES = \
    $(SRC_DIR)/library/Core/Exception.cpp \
    $(SRC_DIR)/library/Core/FileUtils.cpp \
    $(SRC_DIR)/library/Core/Identified.cpp \
    $(SRC_DIR)/library/Core/Indexed.cpp \
    $(SRC_DIR)/library/Core/Math.cpp \
    $(SRC_DIR)/library/Core/Printer.cpp \
    $(SRC_DIR)/library/Core/Random.cpp

LIB_OBJECTS = $(LIB_SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
LIB_TARGET = $(LIB_DIR)/libgem$(SHARED_EXT)

library: $(LIB_TARGET)

$(LIB_TARGET): $(LIB_OBJECTS) $(GLPK_LIB)
	@echo "Building GEM++ library..."
	@mkdir -p $(LIB_DIR)
	$(CXX) $(SHARED_FLAGS) -o $@ $(LIB_OBJECTS) $(GLPK_LIB) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Plugins
PLUGIN_SOURCES = $(SRC_DIR)/plugins/GLPK/GLPKPluginSimple.cpp
PLUGIN_OBJECTS = $(PLUGIN_SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
PLUGIN_TARGET = $(LIB_DIR)/libgemglpk$(SHARED_EXT)

plugins: $(PLUGIN_TARGET)

$(PLUGIN_TARGET): $(PLUGIN_OBJECTS) $(LIB_TARGET)
	@echo "Building GLPK plugin..."
	@mkdir -p $(LIB_DIR)
	$(CXX) $(SHARED_FLAGS) -o $@ $(PLUGIN_OBJECTS) -L$(LIB_DIR) -lgem $(LDFLAGS)

# Applications
CORE_APP_SOURCES = $(SRC_DIR)/apps/Core/main.cpp
CORE_APP_OBJECTS = $(CORE_APP_SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
CORE_APP_TARGET = $(BIN_DIR)/gem-core

apps: $(CORE_APP_TARGET)

$(CORE_APP_TARGET): $(CORE_APP_OBJECTS) $(LIB_TARGET) $(PLUGIN_TARGET)
	@echo "Building core application..."
	@mkdir -p $(BIN_DIR)
	$(CXX) -o $@ $(CORE_APP_OBJECTS) -L$(LIB_DIR) -lgem -lgemglpk $(LDFLAGS)

# Clean
clean:
	rm -rf $(BUILD_DIR)

# Install
install: all
	@echo "Installing to $(PREFIX)..."
	@mkdir -p $(PREFIX)/bin $(PREFIX)/lib $(PREFIX)/include
	cp $(BIN_DIR)/* $(PREFIX)/bin/
	cp $(LIB_DIR)/*.$(SHARED_EXT) $(PREFIX)/lib/
	# Copy headers (simplified)
	find $(SRC_DIR)/library -name "*.h" -exec cp --parents {} $(PREFIX)/include/ \;

# Help
help:
	@echo "Available targets:"
	@echo "  all      - Build everything"
	@echo "  glpk     - Build GLPK library"
	@echo "  library  - Build GEM++ library"
	@echo "  plugins  - Build plugins"
	@echo "  apps     - Build applications"
	@echo "  clean    - Clean build directory"
	@echo "  install  - Install to $(PREFIX)"
	@echo ""
	@echo "Variables:"
	@echo "  BUILD_TYPE=debug|release (default: release)"
	@echo "  PREFIX=/path/to/install (default: /usr/local)"
