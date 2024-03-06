set(SDL2_PATH "/usr/lib/x86_64-linux-gnu/libSDL2.so")
set(GLEW_PATH "/usr/local/lib64/libGLEW.so")


project(t4x_gui2)
add_executable(build/T4X src/main.cpp)
target_link_libraries(build/T4X ${SDL2_PATH} )