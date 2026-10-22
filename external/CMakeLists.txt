# [EXTERNAL]

# GLAD.
add_library(glad STATIC source/glad.c)

target_include_directories(glad PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# GLFW configuration.
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

# Build GLFW as a static library.
set(GLFW_LIBRARY_TYPE STATIC CACHE STRING "" FORCE)

# Process GLFW's own CMakeLists.txt.
include(FetchContent)

FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.5.1
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glfw)