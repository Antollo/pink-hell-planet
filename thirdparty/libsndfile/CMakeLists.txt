set(BUILD_PROGRAMS OFF CACHE BOOL "disable programs" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "disable examples" FORCE)
set(ENABLE_CPACK OFF CACHE BOOL "disable CPack" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "disable tests" FORCE)

add_subdirectory(libsndfile)
