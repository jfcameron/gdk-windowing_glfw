# © Joseph Cameron - MIT License

add_executable(gdkwindowing_glfw_demo "${CMAKE_CURRENT_LIST_DIR}/main.cpp")

target_link_libraries(gdkwindowing_glfw_demo PRIVATE gdkwindowing_glfw)

target_compile_features(gdkwindowing_glfw_demo PRIVATE cxx_std_20)

if (EMSCRIPTEN)
    set_target_properties(gdkwindowing_glfw_demo PROPERTIES SUFFIX ".html")
endif()
