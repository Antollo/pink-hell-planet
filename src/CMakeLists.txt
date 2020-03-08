add_executable(opengl-sample main.cpp)

target_link_libraries(opengl-sample OpenGL::GL glm::glm glfw glad)

#if(MSVC)
#    set_target_properties(opengl-sample PROPERTIES LINK_FLAGS_RELEASE /SUBSYSTEM:WINDOWS)
#endif()