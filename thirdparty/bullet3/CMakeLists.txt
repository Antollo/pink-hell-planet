set(BUILD_UNIT_TESTS OFF CACHE BOOL "disable tests" FORCE)
set(BUILD_CPU_DEMOS OFF CACHE BOOL "disable demos" FORCE)
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "disable demos" FORCE)
set(BUILD_EXTRAS OFF CACHE BOOL "disable extras" FORCE)
set(USE_GLUT OFF CACHE BOOL "disable glut" FORCE)

add_subdirectory(bullet3)

add_library(bullet3 INTERFACE)
target_include_directories(bullet3 INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/bullet3/src")
target_link_libraries(bullet3 INTERFACE LinearMath Bullet3Common BulletInverseDynamics BulletCollision BulletDynamics BulletSoftBody)
