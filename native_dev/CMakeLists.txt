cmake_minimum_required(VERSION 3.18)
project(T4X)

set( TARGET ${PROJECT_NAME} )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build )

set(SDL2_PATH "/usr/lib/x86_64-linux-gnu/libSDL2.so")
set(LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib")

set(SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE CPPS "${SOURCE_PATH}/*.cpp")

add_executable(${TARGET}
    ${CPPS}
)

target_compile_options(${TARGET} PRIVATE -g)

target_include_directories(${TARGET} PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(${TARGET} 
    "${LIB_PATH}/FastNoise2/libFastNoise.so"
    "${LIB_PATH}/FastNoise2/libFastNoiseD.so"
    "${LIB_PATH}/glew/lib64/libGLEW.so"
    "${LIB_PATH}/glew/lib64/libGLEW.so.2.2"
    "${LIB_PATH}/glew/lib64/libGLEW.so.2.2.0"
    "${LIB_PATH}/glm/libglm.so"
    ${SDL2_PATH}
    "-lGLU"
    "-lGL"
)