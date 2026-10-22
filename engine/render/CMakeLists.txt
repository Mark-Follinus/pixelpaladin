add_library(PldnRender STATIC
    animation/animator_2d.cpp
    camera/camera_2d.cpp

    opengl/render_manager_opengl.cpp
    opengl/renderers/2d/renderer_2d_opengl.cpp
    opengl/shaders/shader.cpp

    opengl/textures/texture_2d.cpp
    opengl/textures/texture_manager.cpp
)

target_include_directories(PldnRender PUBLIC
    "${PROJECT_SOURCE_DIR}/engine"
    "${PROJECT_SOURCE_DIR}/external/include"
)

target_include_directories(PldnRender PRIVATE
    "${PROJECT_SOURCE_DIR}/external/include/stb"
)

target_link_libraries(PldnRender PUBLIC
    glad
    PldnGameReplicated
)