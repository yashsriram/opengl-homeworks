# Set the minimum required version of cmake for this project
# this probably could be lower, but I dont have cmake 2.anything to test with
cmake_minimum_required (VERSION 3.1)

# Create a project called 'opengl_basic_template'
project(HW2b)

# Define in the C++ code what the variable "SRC_DIR" should be equal to the current_path/src
add_definitions( -DSRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}/src" )


# Find OpenGL, and set link library names and include paths
find_package(OpenGL REQUIRED)
set(OPENGL_LIBRARIES ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
set(OPENGL_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
include_directories(${OPENGL_INCLUDE_DIRS})

# Also disable building some of the extra things GLFW has (examples, tests, docs)
set(GLFW_BUILD_EXAMPLES  OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_TESTS     OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_DOCS      OFF CACHE BOOL " " FORCE)

# Now actually run cmake on the CMakeLists.txt file found inside of the GLFW directory
add_subdirectory(ext/glfw)

# Make a list of all the source files
set(
    SOURCES
        src/main.cpp
    ext/glad/src/glad.c
)

# Make a list of all the header files
set(
    INCLUDES
        src/shader_compiler.hpp)

# Make a list of all of the directories to look in when doing #include "whatever.h"
set(
    INCLUDE_DIRS
    ext/
    ext/glfw/include
    ext/glad/include
)

set(
    LIBS
    glfw
    ${OPENGL_LIBRARIES}
)

# Actually define what we are trying to produce here (an executable), as
# well as what items are needed to create it (the header and source files)
add_executable(${PROJECT_NAME} ${SOURCES} ${INCLUDES})

# Tell cmake which directories to look in when you #include a file
# Equivalent to the "-I" option for g++
include_directories(${INCLUDE_DIRS})

# Tell cmake which libraries to link to
# Equivalent to the "-l" option for g++
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})

# For Visual Studio only
if (MSVC)
    # Do a parallel compilation of this project
    target_compile_options(${PROJECT_NAME} PRIVATE "/MP")
    # Have this project be the default startup project (the one to build/run when hitting F5)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()