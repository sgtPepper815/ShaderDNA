
add_executable(ShaderDNA
    Main.cpp
    Renderer.h
    Renderer.cpp
    ShaderLoader.h
    Genome.cpp
    Genome.h
    DNA.h
    GUI.h
    GUI.cpp
    utils.h
    Framebuffer.h
    Shaders.h
)

set_target_properties(ShaderDNA PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

include(FetchContent)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE
)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
    GIT_SHALLOW TRUE
)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.9b
    GIT_SHALLOW TRUE
)


FetchContent_MakeAvailable(glfw glm imgui)

find_package(OpenGL REQUIRED)

set(IMGUI_DIR "${imgui_SOURCE_DIR}")

add_library(imgui STATIC
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/imgui_demo.cpp"

    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
)

target_include_directories(imgui PUBLIC
    "${IMGUI_DIR}"
    "${IMGUI_DIR}/backends"
)

target_link_libraries(imgui PUBLIC
    glfw
    OpenGL::GL
)

add_library(glad STATIC
    external/glad/src/glad.c
)

target_include_directories(glad PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include"
)

target_link_libraries(ShaderDNA PRIVATE
    glfw
    glm::glm
    imgui
    glad
    OpenGL::GL
    dwmapi
)

target_compile_definitions(ShaderDNA PRIVATE
    SHADER_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/shader"
    FONT_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts"
)