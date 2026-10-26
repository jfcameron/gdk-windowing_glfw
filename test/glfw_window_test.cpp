// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include <gdk/windowing/glfw_context.h>
#include <gdk/windowing/glfw_window.h>
#include <gdk/windowing/window.h>
#include <gdk/windowing/exception.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <memory>
#include <string_view>
#include <vector>
#include <string>

using namespace gdk::windowing;

TEST_CASE("a window can be made and asked about itself", "[glfw]")
{
    const std::string name("Cool Window");

    auto pWindow = glfw_window::make(name);

    REQUIRE(pWindow);

    SECTION("it reports the name it was given")
    {
        REQUIRE(pWindow->name() == name);
    }

    SECTION("it hands out the implementation handle, which is what input attaches to")
    {
        REQUIRE(pWindow->ptr_to_implementation());
    }

    SECTION("a new window has not been asked to close")
    {
        REQUIRE_FALSE(pWindow->should_close());
    }

    SECTION("and asking it to close is what sets that")
    {
        pWindow->close();

        REQUIRE(pWindow->should_close());
    }

    SECTION("size and aspect ratio agree with each other")
    {
        const auto size = pWindow->window_size();

        REQUIRE(size.first > 0);
        REQUIRE(size.second > 0);
        REQUIRE(pWindow->aspect_ratio()
            == Approx(static_cast<double>(size.first) / static_cast<double>(size.second)));
    }
}

TEST_CASE("the interface is usable without naming the implementation", "[glfw]")
{
    const window_ptr_type pWindow = glfw_window::make("Interface");

    REQUIRE(pWindow);
    REQUIRE(pWindow->name() == "Interface");
    REQUIRE_FALSE(pWindow->should_close());

    pWindow->set_cursor(window::standard_cursor_graphic::ibeam);
    pWindow->set_cursor(window::standard_cursor_graphic::crosshair);

    pWindow->swap_buffers();

    pWindow->close();

    REQUIRE(pWindow->should_close());
}

TEST_CASE("a name that is not null terminated still names the window", "[glfw]")
{
    const std::vector<char> backing{'C', 'o', 'o', 'l', ' ', 'W', 'i', 'n', 'd', 'o', 'w'};
    const std::string_view name(backing.data(), backing.size());

    auto pWindow = glfw_window::make(name);

    REQUIRE(pWindow);
    REQUIRE(pWindow->name() == "Cool Window");
}

TEST_CASE("icons are checked before glfw is given them", "[glfw]")
{
    auto pWindow = glfw_window::make("Icons");

    SECTION("no icons asks for the system default back, rather than indexing an empty vector")
    {
        REQUIRE_NOTHROW(pWindow->set_icons({}));
    }

    SECTION("a well formed icon is accepted")
    {
        window::icon_image_collection_type icons;

        icons.push_back(window::icon_image_type{2, 2,
            std::vector<std::byte>(2 * 2 * window::icon_image_type::CHANNEL_COUNT)});

        REQUIRE_NOTHROW(pWindow->set_icons(icons));
    }

    SECTION("an icon carrying less data than its dimensions claim is refused, not read past")
    {
        window::icon_image_collection_type icons;

        icons.push_back(window::icon_image_type{64, 64, std::vector<std::byte>(4)});

        REQUIRE_THROWS_AS(pWindow->set_icons(icons), exception);
    }
}

TEST_CASE("glfw is initialised once and torn down when the last window is gone", "[glfw]")
{
    SECTION("a window can be made, dropped, and made again")
    {
        {
            auto pFirst = glfw_window::make("First");

            REQUIRE(pFirst);
        }

        auto pSecond = glfw_window::make("Second");

        REQUIRE(pSecond);
        REQUIRE(pSecond->name() == "Second");
    }

    SECTION("an implementation handle outliving its window keeps glfw alive")
    {
        glfw_window::window_pointer_type pHandle;

        {
            auto pWindow = glfw_window::make("Outlived");

            pHandle = pWindow->ptr_to_implementation();
        }

        REQUIRE(pHandle);

        pHandle.reset();

        REQUIRE_NOTHROW(glfw_window::make("After"));
    }
}

TEST_CASE("a window's size is its drawing surface's, as the system gave it", "[glfw][size]")
{
    auto pWindow = glfw_window::make("Sized", {320, 240}, vsync_mode::off);

    const auto framebuffer = [&pWindow] {
        int width = 0, height = 0;
        glfwGetFramebufferSize(pWindow->ptr_to_implementation().get(), &width, &height);
        return window_size_type{width, height};
    };

    REQUIRE(pWindow->window_size() == framebuffer());

    SECTION("and follows a resize nobody told it about")
    {
        glfwSetWindowSize(pWindow->ptr_to_implementation().get(), 400, 300);

        for (int i = 0; i < 20 && pWindow->window_size() != framebuffer(); ++i) glfw_window::poll_events();

        REQUIRE(pWindow->window_size() == framebuffer());
        REQUIRE(pWindow->aspect_ratio() == Approx(aspect_ratio_of(framebuffer())));
    }
}

TEST_CASE("a handle that outlives its window is safe to keep using", "[glfw][lifetime]")
{
    glfw_window::window_pointer_type pHandle;

    {
        auto pWindow = glfw_window::make("Outlived", {320, 240});

        pWindow->set_cursor(window::standard_cursor_graphic::hand);

        pHandle = pWindow->ptr_to_implementation();
    }

    glfwSetWindowSize(pHandle.get(), 360, 280);

    for (int i = 0; i < 10; ++i) REQUIRE_NOTHROW(glfw_window::poll_events());

    int width = 0, height = 0;
    glfwGetWindowSize(pHandle.get(), &width, &height);

    REQUIRE(width > 0);
}

TEST_CASE("a custom cursor does not outlive glfw", "[glfw][lifetime]")
{
    {
        auto pWindow = glfw_window::make("Cursor");

        pWindow->set_cursor(window::cursor_image_type{});
    }

    auto pAfter = glfw_window::make("After a cursor");

    pAfter->set_cursor(window::standard_cursor_graphic::ibeam);

    REQUIRE(pAfter->name() == "After a cursor");
}

TEST_CASE("a context keeps glfw up, so it can poll before any window exists", "[glfw][context]")
{
    const auto pContext = glfw_context::make();

    REQUIRE_NOTHROW(pContext->poll_events());

    const auto pWindow = pContext->make_window("Later");

    REQUIRE_NOTHROW(pContext->poll_events());
}

TEST_CASE("the glfw context drives real windows through the interface", "[glfw][context]")
{
    const context_ptr_type pContext = glfw_context::make();

    const window_ptr_type pWindow = pContext->make_window("Framed", {320, 240}, vsync_mode::off);

    REQUIRE(pWindow);
    REQUIRE(pWindow->name() == "Framed");

    int frames = 0;

    while (!pWindow->should_close()) {
        pContext->poll_events();

        pWindow->swap_buffers();

        if (++frames >= 5) pWindow->close();
    }

    REQUIRE(frames == 5);
}

TEST_CASE("two windows exist at the same time", "[glfw][multiwindow]")
{
    auto pFirst = glfw_window::make("First", {320, 240});
    auto pSecond = glfw_window::make("Second", {400, 300});

    REQUIRE(pFirst);
    REQUIRE(pSecond);

    SECTION("each keeps its own identity")
    {
        REQUIRE(pFirst->name() == "First");
        REQUIRE(pSecond->name() == "Second");

        REQUIRE(pFirst->window_size() != pSecond->window_size());
    }

    SECTION("and its own handle, which is what anything drawing into it attaches to")
    {
        REQUIRE(pFirst->ptr_to_implementation() != pSecond->ptr_to_implementation());
    }

    SECTION("closing one leaves the other alone")
    {
        pFirst->close();

        REQUIRE(pFirst->should_close());
        REQUIRE_FALSE(pSecond->should_close());
    }

    SECTION("both can be presented, in either order")
    {
        REQUIRE_NOTHROW(pSecond->swap_buffers());
        REQUIRE_NOTHROW(pFirst->swap_buffers());
        REQUIRE_NOTHROW(pSecond->swap_buffers());
    }

    SECTION("polling serves both, and neither owns it")
    {
        auto pContext = glfw_context::make();

        REQUIRE_NOTHROW(pContext->poll_events());

        REQUIRE_FALSE(pFirst->should_close());
        REQUIRE_FALSE(pSecond->should_close());
    }
}

#ifndef __EMSCRIPTEN__
TEST_CASE("the glfw session outlives individual windows", "[glfw][multiwindow]")
{
    const auto glfw_is_initialised = [](GLFWwindow *const apWindow) {
        glfwGetError(nullptr);                            

        glfwGetWindowAttrib(apWindow, GLFW_FOCUSED);

        return glfwGetError(nullptr) != GLFW_NOT_INITIALIZED;
    };

    auto pFirst = glfw_window::make("Outlives", {320, 240});

    auto *const pFirstHandle = pFirst->ptr_to_implementation().get();

    REQUIRE(glfw_is_initialised(pFirstHandle));

    {
        auto pSecond = glfw_window::make("Temporary");

        REQUIRE(pSecond->name() == "Temporary");
    }

    REQUIRE(glfw_is_initialised(pFirstHandle));

    REQUIRE_FALSE(pFirst->should_close());
    REQUIRE_NOTHROW(pFirst->swap_buffers());

    SECTION("and a window can be made after the last one was released")
    {
        pFirst.reset();

        auto pLater = glfw_window::make("After", {320, 240});

        REQUIRE(pLater);
        REQUIRE(glfw_is_initialised(pLater->ptr_to_implementation().get()));
        REQUIRE_NOTHROW(pLater->swap_buffers());
    }
}
#endif

TEST_CASE("a second window takes the current context", "[glfw][multiwindow]")
{
    auto pFirst = glfw_window::make("First");

    REQUIRE(glfwGetCurrentContext() == pFirst->ptr_to_implementation().get());

    auto pSecond = glfw_window::make("Second");

    REQUIRE(glfwGetCurrentContext() == pSecond->ptr_to_implementation().get());
    REQUIRE(glfwGetCurrentContext() != pFirst->ptr_to_implementation().get());
}
