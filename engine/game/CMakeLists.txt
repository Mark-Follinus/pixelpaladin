add_library(PldnGameAuthoritative STATIC
    world/authoritative/game_world_authoritative.cpp
)

add_library(PldnGameReplicated STATIC
    world/replicated/game_world_replicated.cpp
)

foreach(target IN ITEMS PldnGameAuthoritative PldnGameReplicated)
    target_include_directories(${target} PUBLIC
        "${PROJECT_SOURCE_DIR}/engine"
        "${PROJECT_SOURCE_DIR}/external/include"
    )
endforeach()