# © Joseph Cameron - MIT License

if (NOT TARGET gdkwindowing)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/gdk-windowing")
endif()

if (TARGET glfw)
    return()
endif()

if (EMSCRIPTEN)
    add_library(glfw INTERFACE)

    target_link_options(glfw INTERFACE "-sUSE_GLFW=3")

    return()
endif()

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

set(_gdkwindowing_glfw_shared "${BUILD_SHARED_LIBS}")
set(BUILD_SHARED_LIBS OFF)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/glfw")

set(BUILD_SHARED_LIBS "${_gdkwindowing_glfw_shared}")
