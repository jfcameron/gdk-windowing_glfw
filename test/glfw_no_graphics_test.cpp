// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include <gdk/windowing/glfw_context.h>
#include <gdk/windowing/glfw_window.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <variant>

using namespace gdk::windowing;

namespace {
    [[nodiscard]] std::shared_ptr<glfw_window> without_graphics(const std::string_view aName) {
        return glfw_window::make(aName, {400, 300}, vsync_mode::adaptive, glfw_window::graphics_api::none);
    }
}

TEST_CASE("a window with no graphics API makes no context and takes none", "[glfw][no graphics]") {
    const auto pOpenGL = glfw_window::make("with a context");
    const auto pWindow = without_graphics("without one");

    REQUIRE(pWindow->graphics() == glfw_window::graphics_api::none);
    REQUIRE(glfwGetWindowAttrib(pWindow->ptr_to_implementation().get(), GLFW_CLIENT_API) == GLFW_NO_API);
    REQUIRE(glfwGetCurrentContext() == pOpenGL->ptr_to_implementation().get());

    SECTION("its renderer presents, so swapping its buffers does nothing and glfw reports no error") {
        pWindow->swap_buffers();

        REQUIRE_NOTHROW(glfw_window::poll_events());
    }

    SECTION("the next window made with OpenGL has a context again") {
        const auto pNext = glfw_window::make("after");

        REQUIRE(glfwGetWindowAttrib(pNext->ptr_to_implementation().get(), GLFW_CLIENT_API) == GLFW_OPENGL_API);
        REQUIRE(glfwGetCurrentContext() == pNext->ptr_to_implementation().get());
    }
}

TEST_CASE("a window with no graphics API is never offered as a context to share", "[glfw][no graphics]") {
    auto pOpenGL = glfw_window::make("shares its context");
    const auto pWindow = without_graphics("has none to share");

    pOpenGL.reset();

    REQUIRE_NOTHROW(glfw_window::make("made while only the one without a context lives"));
}

TEST_CASE("a context made for no graphics API makes its windows without one", "[glfw][no graphics][context]") {
    const auto pContext = glfw_context::make(glfw_window::graphics_api::none);
    const auto pWindow = pContext->make_window("through the interface");

    const auto *const pGLFW = dynamic_cast<const glfw_window *>(pWindow.get());

    REQUIRE(pGLFW);
    REQUIRE(pGLFW->graphics() == glfw_window::graphics_api::none);
}

#ifndef __EMSCRIPTEN__
TEST_CASE("a window's native handles are those of the system glfw runs on", "[glfw][handles]") {
    const auto graphics = GENERATE(glfw_window::graphics_api::none, glfw_window::graphics_api::opengl);
    const auto pWindow = glfw_window::make("handles", {400, 300}, vsync_mode::adaptive, graphics);
    const auto handles = pWindow->handles();

    switch (glfwGetPlatform()) {
        case GLFW_PLATFORM_X11: {
            const auto *const pX11 = std::get_if<x11_handles>(&handles);

            REQUIRE(pX11);
            REQUIRE(pX11->display);
            REQUIRE(pX11->window);
        } break;

        case GLFW_PLATFORM_WAYLAND: {
            const auto *const pWayland = std::get_if<wayland_handles>(&handles);

            REQUIRE(pWayland);
            REQUIRE(pWayland->display);
            REQUIRE(pWayland->surface);
        } break;

        case GLFW_PLATFORM_WIN32: {
            const auto *const pWin32 = std::get_if<win32_handles>(&handles);

            REQUIRE(pWin32);
            REQUIRE(pWin32->instance);
            REQUIRE(pWin32->window);
        } break;

        case GLFW_PLATFORM_COCOA: {
            const auto *const pCocoa = std::get_if<cocoa_handles>(&handles);

            REQUIRE(pCocoa);
            REQUIRE(pCocoa->window);
        } break;

        default: FAIL("glfw runs on platform " << glfwGetPlatform());
    }

    SECTION("and two windows have different ones") {
        const auto pOther = glfw_window::make("other", {400, 300}, vsync_mode::adaptive, graphics);

        REQUIRE(pOther->handles() != handles);
    }
}
#endif
