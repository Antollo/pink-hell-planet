SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin )
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin )
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin )

add_executable(game
    main.cpp
    Terrain.cpp
    marchingCubesTable.cpp
    World.cpp
    RigidBody.cpp
    GhostObject.cpp
    CollisionObject.cpp
)

target_link_libraries(game OpenGL::GL glm::glm glfw glad tinyobjloader lodepng bullet3 BulletDynamics BulletCollision freetype)

#if(MSVC)
#    set_target_properties(opengl-sample PROPERTIES LINK_FLAGS_RELEASE /SUBSYSTEM:WINDOWS)
#endif()
