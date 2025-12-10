cmake_minimum_required(VERSION 3.10)
project(gempp-sub C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

# Find required packages
find_package(PkgConfig REQUIRED)

# Find Qt5
find_package(Qt5 REQUIRED COMPONENTS Core Xml Gui Widgets)

# Try to find GLPK
find_library(GLPK_LIBRARY NAMES glpk)
find_path(GLPK_INCLUDE_DIR NAMES glpk.h)

# Try to find libgem++
find_library(GEMPP_LIBRARY NAMES GEM++ libGEM++)
find_path(GEMPP_INCLUDE_DIR NAMES Model/Graph.h Formulation/SubgraphMatching.h)

# Platform-specific settings
if(WIN32)
    # Windows-specific paths and libraries
    if(NOT GLPK_LIBRARY)
        set(GLPK_LIBRARY "C:/Program Files/GLPK/lib/glpk.lib")
    endif()
    if(NOT GLPK_INCLUDE_DIR)
        set(GLPK_INCLUDE_DIR "C:/Program Files/GLPK/include")
    endif()

    if(NOT GEMPP_LIBRARY)
        set(GEMPP_LIBRARY "${CMAKE_SOURCE_DIR}/../src/libGEM++.lib")
    endif()
    if(NOT GEMPP_INCLUDE_DIR)
        set(GEMPP_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../src/library")
    endif()

    # Additional Windows libraries
    set(EXTRA_LIBS ws2_32)

elseif(APPLE)
    # macOS-specific paths
    if(NOT GLPK_LIBRARY)
        set(GLPK_LIBRARY "/usr/local/lib/libglpk.dylib")
    endif()
    if(NOT GLPK_INCLUDE_DIR)
        set(GLPK_INCLUDE_DIR "/usr/local/include")
    endif()

    if(NOT GEMPP_LIBRARY)
        set(GEMPP_LIBRARY "${CMAKE_SOURCE_DIR}/../src/libGEM++.dylib")
    endif()
    if(NOT GEMPP_INCLUDE_DIR)
        set(GEMPP_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../src/library")
    endif()

    # Additional macOS libraries
    set(EXTRA_LIBS "")

else()
    # Linux/Unix
    if(NOT GLPK_LIBRARY)
        set(GLPK_LIBRARY "/usr/lib/libglpk.so")
    endif()
    if(NOT GLPK_INCLUDE_DIR)
        set(GLPK_INCLUDE_DIR "/usr/include")
    endif()

    if(NOT GEMPP_LIBRARY)
        set(GEMPP_LIBRARY "${CMAKE_SOURCE_DIR}/../src/libGEM++.so")
    endif()
    if(NOT GEMPP_INCLUDE_DIR)
        set(GEMPP_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../src/library")
    endif()

    # Additional Linux libraries
    set(EXTRA_LIBS "")
endif()

# Check if libraries were found
if(NOT GLPK_LIBRARY)
    message(WARNING "GLPK library not found automatically. Please install GLPK or set GLPK_LIBRARY manually.")
    message("On macOS: brew install glpk")
    message("On Ubuntu: sudo apt-get install libglpk-dev")
    message("On Windows: Download from https://sourceforge.net/projects/winglpk/")
endif()

if(NOT GEMPP_LIBRARY)
    message(WARNING "libGEM++ library not found. Please build libGEM++ first from the main project.")
    message("Run 'qmake GEM++.pro && make' in the root directory first.")
endif()

# Include directories
include_directories(${GLPK_INCLUDE_DIR})
include_directories(${GEMPP_INCLUDE_DIR})

# Source files
set(SOURCES main.c gempp_wrapper.cpp)

# Create executable
add_executable(gempp-sub ${SOURCES})

# Enable Qt automoc for the wrapper
set_target_properties(gempp-sub PROPERTIES AUTOMOC ON)

# Define that we're using the GEM++ library (not building it)
add_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII)

# Link libraries
target_link_libraries(gempp-sub
    ${GEMPP_LIBRARY}
    ${GLPK_LIBRARY}
    Qt5::Core
    Qt5::Xml
    Qt5::Gui
    Qt5::Widgets
    ${EXTRA_LIBS}
)

# Compiler flags
if(MSVC)
    target_compile_options(gempp-sub PRIVATE /W4)
else()
    target_compile_options(gempp-sub PRIVATE -Wall -Wextra -pedantic)
endif()

# Install target
install(TARGETS gempp-sub DESTINATION bin)