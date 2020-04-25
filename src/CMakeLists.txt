add_executable(game main.cpp)

target_link_libraries(game OpenGL::GL glm::glm glfw glad tinyobjloader)

#if(MSVC)
#    set_target_properties(opengl-sample PROPERTIES LINK_FLAGS_RELEASE /SUBSYSTEM:WINDOWS)
#endif()