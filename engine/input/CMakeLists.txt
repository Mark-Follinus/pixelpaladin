add_library(PldnInput STATIC
    windows/input_windows.cpp
)

target_include_directories(PldnInput PUBLIC
    "${PROJECT_SOURCE_DIR}/engine"
    "${PROJECT_SOURCE_DIR}/external/include"
)

target_link_libraries(PldnInput PUBLIC
    glfw
)