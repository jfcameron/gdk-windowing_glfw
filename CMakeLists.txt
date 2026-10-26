# © Joseph Cameron - MIT License

cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

if (TARGET gdkwindowing_glfw)
    return()
endif()

project(gdk-windowing_glfw
    VERSION 1.0
    DESCRIPTION "the gdk window interface, implemented over GLFW"
    LANGUAGES C CXX)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/jfc-cmake/jfclib.cmake")

option(GDKWINDOWING_GLFW_BUILD_TESTS "Build unit tests; they need a display" ${PROJECT_IS_TOP_LEVEL})
option(GDKWINDOWING_GLFW_BUILD_DEMO "Build the demo" OFF)
option(GDKWINDOWING_GLFW_BUILD_DOCS "Build documentation" OFF)

add_subdirectory(extern)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/permissive->)
else()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-pedantic>)
    endif()

    include(CheckCXXCompilerFlag)

    foreach (_strict_flag "-Werror=c++20-extensions" "-Werror=c++23-extensions")
        string(MAKE_C_IDENTIFIER "HAS${_strict_flag}" _strict_flag_symbol)

        check_cxx_compiler_flag("${_strict_flag}" ${_strict_flag_symbol})

        if (${_strict_flag_symbol})
            add_compile_options($<$<COMPILE_LANGUAGE:CXX>:${_strict_flag}>)
        endif()
    endforeach()
endif()

add_library(gdkwindowing_glfw STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/glfw_context.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/glfw_session.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/glfw_window.cpp")

add_library(gdk::windowing_glfw ALIAS gdkwindowing_glfw)

add_library(gdkwindow ALIAS gdkwindowing_glfw)
add_library(gdk::window ALIAS gdkwindowing_glfw)

set_target_properties(gdkwindowing_glfw PROPERTIES EXPORT_NAME windowing_glfw PREFIX "lib")

target_include_directories(gdkwindowing_glfw
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/include")

target_compile_features(gdkwindowing_glfw PUBLIC cxx_std_20)

target_link_libraries(gdkwindowing_glfw PUBLIC gdkwindowing glfw)

if (GDKWINDOWING_GLFW_BUILD_TESTS)
    add_subdirectory(test)
endif()

if (GDKWINDOWING_GLFW_BUILD_DEMO)
    add_subdirectory(demo)
endif()

if (GDKWINDOWING_GLFW_BUILD_DOCS)
    add_subdirectory(docs)
endif()
