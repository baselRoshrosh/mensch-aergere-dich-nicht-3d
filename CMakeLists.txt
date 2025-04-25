cmake_minimum_required(VERSION 3.15)
project(MenschAergereDichNicht LANGUAGES C CXX)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------------
# Dependencies via FetchContent
# -----------------------------------------------------------------------------
include(FetchContent)

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.8
)
FetchContent_MakeAvailable(glfw)

# GLM (header-only math library)
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        0.9.9.8
)
FetchContent_MakeAvailable(glm)

# OpenGL
find_package(OpenGL REQUIRED)

# -----------------------------------------------------------------------------
# Build glad (your vendored loader)
# -----------------------------------------------------------------------------
add_library(glad STATIC
  src/lib/glad/glad.c
)
target_include_directories(glad PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# -----------------------------------------------------------------------------
# Build GLRender (your small render layer)
# -----------------------------------------------------------------------------
file(GLOB GLRENDER_SRCS
  src/lib/GLRender/*.cpp
)
add_library(GLRender STATIC ${GLRENDER_SRCS})
target_include_directories(GLRender PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(GLRender PUBLIC glad)

# -----------------------------------------------------------------------------
# Build your game executable
# -----------------------------------------------------------------------------
set(APP_SOURCES
  src/Board.cpp
  src/Figur.cpp
  src/sample/GLSample/GLSample.cpp
)

add_executable(MenschAergereDichNicht ${APP_SOURCES})
target_include_directories(MenschAergereDichNicht PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(MenschAergereDichNicht
  PRIVATE
    glad
    GLRender
    glfw
    OpenGL::GL
    glm
)

# -----------------------------------------------------------------------------
# Copy shaders & textures next to the built binary
# -----------------------------------------------------------------------------
add_custom_command(TARGET MenschAergereDichNicht POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_CURRENT_SOURCE_DIR}/shader
          $<TARGET_FILE_DIR:MenschAergereDichNicht>/shader
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_CURRENT_SOURCE_DIR}/textures
          $<TARGET_FILE_DIR:MenschAergereDichNicht>/textures
)
