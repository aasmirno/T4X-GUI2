cmake_minimum_required(VERSION 3.18)
project(T4X)

macro(get_win_hex outvar)
  string(REGEX MATCH "^([0-9]+)\\.([0-9]+)" ${outvar} ${CMAKE_SYSTEM_VERSION})
  math(EXPR ${outvar} "(${CMAKE_MATCH_1} << 8) + ${CMAKE_MATCH_2}" OUTPUT_FORMAT HEXADECIMAL)
endmacro()

if(WIN32)
  get_win_hex(winver)
  add_compile_definitions(_WIN32_WINNT=${winver})
endif()

set( TARGET ${PROJECT_NAME} )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build )


set(LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/LINUX")
set(SDL2_PATH "/usr/lib/x86_64-linux-gnu/libSDL2.so")


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
        ${SDL2_PATH}
        "-lGLU"
        "-lGL"
)
