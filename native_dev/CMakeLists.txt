cmake_minimum_required(VERSION 3.18)
project(T4X)

set( TARGET ${PROJECT_NAME} )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build )

set(SDL2_PATH "/usr/lib/x86_64-linux-gnu/libSDL2.so")
set(GLEW_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/glew/lib64")

set(SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE CPPS "${SOURCE_PATH}/*.cpp")

add_executable(${TARGET}
    ${CPPS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Vector.h
)

target_include_directories(${TARGET} PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(${TARGET} 
    "${GLEW_PATH}/libGLEW.so"
    "${GLEW_PATH}/libGLEW.so.2.2"
    "${GLEW_PATH}/libGLEW.so.2.2.0"
    ${SDL2_PATH}
    "-lGLU"
    "-lGL"
)