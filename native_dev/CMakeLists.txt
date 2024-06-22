cmake_minimum_required(VERSION 3.20)
project(T4X)
set( TARGET ${PROJECT_NAME} )

# set output path
cmake_path(SET CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")

#set library path
if(WIN32)   # windows target
    cmake_path(SET LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/WIN")
    cmake_path(SET SDL2_PATH "${LIB_PATH}/")
else()      # linux target
    cmake_path(SET LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/LINUX")
    cmake_path(SET SDL2_PATH "/usr/lib/x86_64-linux-gnu/libSDL2.so")
endif()

# add cpp sources
cmake_path(SET SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE CPPS "${SOURCE_PATH}/*.cpp")
add_executable(${TARGET} 
    ${CPPS}
    "include/T4X/tdefs.h"
)

# set compile options
target_compile_options(${TARGET} PRIVATE -g)

# set include path
target_include_directories(${TARGET} PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# link libraries
find_package(OpenGL REQUIRED)
if(WIN32)   # windows target
    target_link_libraries(${TARGET}
        "${LIB_PATH}/FastNoise2/FastNoise.lib"
        "${LIB_PATH}/FastNoise2/FastNoiseD.lib"
        "${LIB_PATH}/glew-2.2.0/x64/glew32.lib"
        "${LIB_PATH}/SDL2/x64/SDL2main.lib"
        "${LIB_PATH}/SDL2/x64/SDL2.lib"
        "${OPENGL_gl_LIBRARY}"
    )
else()      # linux target
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
endif()

if(WIN32)
    add_custom_command(TARGET T4X
        POST_BUILD
        COMMAND powershell ./copy_rsc.ps1
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
endif()
