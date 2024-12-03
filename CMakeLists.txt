cmake_minimum_required(VERSION 3.10)
project(checkers)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
# Set OpenGL preference to GLVND
set(OpenGL_GL_PREFERENCE GLVND)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

# GLM is header-only, just need to include its path
find_path(GLM_INCLUDE_DIR glm/glm.hpp
    PATHS
    /usr/include
    /usr/local/include
)

if(NOT GLM_INCLUDE_DIR)
    message(FATAL_ERROR "GLM headers not found")
endif()

# Add source files
set(SOURCES
    main.cpp
    Board.cpp
    Game.cpp
    Renderer.cpp
)

# Add header files
set(HEADERS
    Board.h
    Game.h
    Renderer.h
    Logger.h
)

# Create executable
add_executable(checkers ${SOURCES})

# Link libraries
target_link_libraries(checkers
    OpenGL::GL
    GLEW::GLEW
    glfw
    GLU
)

# Include directories
target_include_directories(checkers PRIVATE
    ${OPENGL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIR}
)
