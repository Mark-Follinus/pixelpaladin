add_library(PldnNetwork STATIC
    windows/networking_runtime_windows.cpp
    windows/connection/connection_windows.cpp

    windows/roles/client/client_windows.cpp
    windows/roles/server/server_windows.cpp
)

target_include_directories(PldnNetwork PUBLIC
    "${PROJECT_SOURCE_DIR}/engine"
    "${PROJECT_SOURCE_DIR}/external/include"
)

target_link_libraries(PldnNetwork PRIVATE
    ws2_32
)