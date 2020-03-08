cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(opengl-sample)

if(MSVC)
    add_compile_options(/O2 /W3 /MT)
else()
    add_compile_options(-O3 -static-libgcc -static-libstdc++)
endif()

find_package(OpenGL REQUIRED)
add_subdirectory(thirdparty/glad)
add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/glm)
add_subdirectory(src)