cmake_minimum_required(VERSION 3.10)
project(gempp-sub C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

# Find required packages
find_package(PkgConfig REQUIRED)

# Find Qt5
find_package(Qt5 REQUIRED COMPONENTS Core Xml Gui Widgets)

# Use local GLPK and GEM++ libraries
set(GLPK_LIBRARY "${CMAKE_SOURCE_DIR}/glpk-4.65/w32/glpk_4_65.lib")
set(GLPK_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/glpk-4.65/src")

# Use local GEM++ library (link directly to DLL)
set(GEMPP_LIBRARY "${CMAKE_SOURCE_DIR}/GEM++/GEM++.dll")
set(GEMPP_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../src/library")

# Platform-specific settings
if(WIN32)
    # Additional Windows libraries
    set(EXTRA_LIBS ws2_32)
else()
    # Additional libraries for other platforms
    set(EXTRA_LIBS "")
endif()

# Check if local libraries exist
if(NOT EXISTS "${GLPK_LIBRARY}")
    message(FATAL_ERROR "GLPK library not found at ${GLPK_LIBRARY}. Please ensure GLPK is built in glpk-4.65/w32/")
endif()

if(NOT EXISTS "${GEMPP_LIBRARY}")
    message(FATAL_ERROR "GEM++ library not found at ${GEMPP_LIBRARY}. Please ensure GEM++ is available in GEM++/ directory.")
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

# Copy required DLLs to output directory
if(WIN32)
    add_custom_command(TARGET gempp-sub POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/glpk-4.65/w32/glpk_4_65.dll"
            $<TARGET_FILE_DIR:gempp-sub>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/GEM++/GEM++.dll"
            $<TARGET_FILE_DIR:gempp-sub>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/GEM++/Qt5Core.dll"
            $<TARGET_FILE_DIR:gempp-sub>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/GEM++/Qt5Xml.dll"
            $<TARGET_FILE_DIR:gempp-sub>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/GEM++/Qt5Gui.dll"
            $<TARGET_FILE_DIR:gempp-sub>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/GEM++/Qt5Widgets.dll"
            $<TARGET_FILE_DIR:gempp-sub>
    )
endif()

# Install target
install(TARGETS gempp-sub DESTINATION bin)