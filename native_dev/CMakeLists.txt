set(SDL2_PATH "/usr/lib/x86_64-linux-gnu/libSDL2.so")
set(GLEW_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/glew/lib64")


project(t4x_gui2)
add_executable(build/T4X 
    src/main.cpp 
    src/render/Renderer.cpp 
    src/render/RenderObject.cpp
    src/render/ShapeObject.cpp
)

target_include_directories(build/T4X PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(build/T4X 
    "${GLEW_PATH}/libGLEW.so"
    "${GLEW_PATH}/libGLEW.so.2.2"
    "${GLEW_PATH}/libGLEW.so.2.2.0"
    ${SDL2_PATH}
    "-lGLU"
    "-lGL"
)

message("${CMAKE_CURRENT_SOURCE_DIR}/include")