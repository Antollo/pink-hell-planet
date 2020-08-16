set(BUILD_SHARED_LIBS OFF CACHE BOOL "build static libs" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "disable examples" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "disable tests" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "don't build docs" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "don't install" FORCE)
set(GLFW_VULKAN_STATIC OFF CACHE BOOL "disable vulcan" FORCE)

add_subdirectory(glfw)
