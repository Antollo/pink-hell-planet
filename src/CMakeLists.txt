set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin )
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin )

add_compile_definitions(GLFW_INCLUDE_NONE)

add_executable(game
    main.cpp
    Terrain.cpp
    marchingCubesTable.cpp
    World.cpp
    RigidBody.cpp
    GhostObject.cpp
    CollisionObject.cpp
    ParticleSystem.cpp
    Bullet.cpp
    PlayableObject.cpp
)

target_link_libraries(game
    OpenGL::GL
    glm::glm
    glfw
    glad
    tinyobjloader
    lodepng
    bullet3
    BulletDynamics
    BulletCollision
    freetype
    nlohmann_json::nlohmann_json
    sndfile
    portaudio_static
    OpenMP::OpenMP_CXX
)

#if(MSVC)
#    set_target_properties(game PROPERTIES LINK_FLAGS_RELEASE /SUBSYSTEM:WINDOWS)
#endif()
