cmake_minimum_required(VERSION 3.18)
project(T4X)

set( TARGET ${PROJECT_NAME} )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build )

set(SDL2_PATH "/usr/lib/x86_64-linux-gnu/libSDL2.so")
set(LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib")

set(SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE CPPS_RENDER "${SOURCE_PATH}/render/*.cpp")
file(GLOB_RECURSE CPPS_GAME "${SOURCE_PATH}/game/*.cpp")
file(GLOB_RECURSE CPPS_INPUT "${SOURCE_PATH}/input/*.cpp")
file(GLOB_RECURSE CPPS_PNG "${SOURCE_PATH}/PNG/*.cpp")

add_executable(${TARGET}
    "${SOURCE_PATH}/main.cpp"
    ${CPPS_PNG}
    ${CPPS_RENDER}
    ${CPPS_INPUT}
    ${CPPS_GAME}
)

target_include_directories(${TARGET} PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(${TARGET} 
    "${LIB_PATH}/glew/lib64/libGLEW.so"
    "${LIB_PATH}/glew/lib64/libGLEW.so.2.2"
    "${LIB_PATH}/glew/lib64/libGLEW.so.2.2.0"
    "${LIB_PATH}/glm/libglm.so"
    ${SDL2_PATH}
    "-lGLU"
    "-lGL"
)