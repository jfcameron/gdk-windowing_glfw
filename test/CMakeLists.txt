# © Joseph Cameron - MIT License

jfc_add_tests(
    C++_STANDARD 20

    TEST_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/glfw_window_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/glfw_conformance.cpp"

    LIBRARIES
        gdkwindowing_glfw
        gdkwindowing_conformance)

set_property(TARGET ${JFC_TEST_TARGET} PROPERTY CXX_EXTENSIONS OFF)
