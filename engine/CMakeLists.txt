# Load the engine modules.
add_subdirectory(game)
add_subdirectory(network)
add_subdirectory(device)
add_subdirectory(input)
add_subdirectory(render)

# Standalone executable.
add_executable(PixelPaladinStandalone
    main.cpp
    core/standalone/engine.cpp
)
target_compile_definitions(PixelPaladinStandalone PRIVATE PLDN_BUILD_MODE=0)
target_include_directories(PixelPaladinStandalone PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(PixelPaladinStandalone PRIVATE
    # TODO: Add standalone dependencies after reviewing core/standalone/engine.cpp.
)

# Client executable.
add_executable(PixelPaladinClient
    main.cpp
    core/client/client_engine.cpp
)
target_compile_definitions(PixelPaladinClient PRIVATE PLDN_BUILD_MODE=1)
target_include_directories(PixelPaladinClient PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(PixelPaladinClient PRIVATE
    PldnGameReplicated
    PldnNetwork
    PldnDevice
    PldnInput
    PldnRender
)

# Server executable.
add_executable(PixelPaladinServer
    main.cpp
    core/server/server_engine.cpp
)
target_compile_definitions(PixelPaladinServer PRIVATE PLDN_BUILD_MODE=2)
target_include_directories(PixelPaladinServer PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(PixelPaladinServer PRIVATE
    PldnGameAuthoritative
    PldnNetwork
)