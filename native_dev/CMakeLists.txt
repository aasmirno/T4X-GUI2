cmake_minimum_required(VERSION 3.20)
project(T4X)
set( TARGET ${PROJECT_NAME} )

# set output path
cmake_path(SET CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")

#set library paths
if(WIN32)   # windows target
    cmake_path(SET LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/WIN")
    cmake_path(SET SDL2_PATH "${LIB_PATH}/")
else()      # linux target
    cmake_path(SET LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/LINUX")
#    find_package(SDL2 REQUIRED)
endif()

# add cpp sources
cmake_path(SET SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE CPPS "${SOURCE_PATH}/*.cpp")
add_executable(${TARGET} 
    ${CPPS}
    )

# set compile options
if(WIN32)   # windows target

else()      # linux target
    target_compile_options(${TARGET} PRIVATE -g )
endif()


# set include path
target_include_directories(${TARGET} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/local"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/shared"
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

    # windows includes
    target_include_directories(${TARGET} PUBLIC 
        "${CMAKE_CURRENT_SOURCE_DIR}/include/local"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/shared"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/win/"
    )
else()      # linux target
    target_link_libraries(${TARGET}
        "${LIB_PATH}/FastNoise2/libFastNoise.so"
        "${LIB_PATH}/FastNoise2/libFastNoiseD.so"
        "${LIB_PATH}/glew/lib64/libGLEW.so"
        "${LIB_PATH}/glew/lib64/libGLEW.so.2.2"
        "${LIB_PATH}/glew/lib64/libGLEW.so.2.2.0"
        "${LIB_PATH}/SDL2/lib/libSDL2-2.0.so.0"
        "${LIB_PATH}/SDL2/lib/libSDL2.so"
        "-lGLU"
        "-lGL"
        "-ldl"
       )
    # linux includes
    target_include_directories(${TARGET} PUBLIC 
        "${CMAKE_CURRENT_SOURCE_DIR}/include/local"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/shared"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/lin/"
    )
endif()

message(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# link and copy resources
cmake_path(SET RESOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/resources")
file(GLOB_RECURSE RESOURCES "${RESOURCE_PATH}/*")
foreach(file ${RESOURCES})
    configure_file(${file} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COPYONLY)
endforeach()