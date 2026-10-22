add_library(PldnDevice STATIC
    windows/device_windows.cpp
)

target_include_directories(PldnDevice PUBLIC
    "${PROJECT_SOURCE_DIR}/engine"
    "${PROJECT_SOURCE_DIR}/external/include"
)

target_link_libraries(PldnDevice PUBLIC
    glad
    glfw
)