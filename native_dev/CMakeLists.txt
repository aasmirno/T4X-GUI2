set(SDL2_PATH "/usr/lib/x86_64-linux-gnu/libSDL2.so")
set(GLEW_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/glew/lib64/libGLEW.so")
set(GLEW_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/lib/glew/include")


project(t4x_gui2)
add_executable(build/T4X src/main.cpp)
target_link_libraries(build/T4X ${SDL2_PATH} ${GLEW_PATH})
target_include_directories(build/T4X PUBLIC ${GLEW_INCLUDE})
message(" ${GLEW_INCLUDE} ")