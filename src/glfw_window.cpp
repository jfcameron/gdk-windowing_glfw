// © Joseph Cameron - MIT License

#include <gdk/windowing/glfw_window.h>

#include <gdk/windowing/exception.h>
#include <gdk/windowing/impl_glfw_session.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

using namespace gdk::windowing;

namespace {
    void destroy_cursor(GLFWcursor *const apCursor) {
        if (apCursor) glfwDestroyCursor(apCursor);
    }

    [[nodiscard]] glfw_window::window_pointer_type create_glfw_window(const std::string_view aName,
        const window_size_type aWindowSize, const vsync_mode aVsync) {
        auto pSession = glfw_detail::acquire_session();

        const std::string name(aName);

        glfw_detail::clear_error();

        GLFWwindow *const pRaw = glfwCreateWindow(aWindowSize.first, aWindowSize.second, name.c_str(),
            nullptr, pSession->share_root());

        if (!pRaw)
            throw exception(std::string("gdk::windowing::glfw/glfwCreateWindow failed. Can the environment "
                "provide an OpenGL context? glfw said: ").append(glfw_detail::take_error().value_or("nothing")));

        pSession->joined(pRaw);

        glfw_window::window_pointer_type pWindow(pRaw, [pSession](GLFWwindow *const apWindow) {
            pSession->left(apWindow);

            glfwDestroyWindow(apWindow);
        });

        glfwMakeContextCurrent(pRaw);

        switch (aVsync) {
            case vsync_mode::off: glfwSwapInterval(0); break;
            case vsync_mode::on: glfwSwapInterval(1); break;

            case vsync_mode::adaptive: {
                glfwSwapInterval(-1);

                if (glfw_detail::take_error()) glfwSwapInterval(1);
            } break;
        }

#ifndef __EMSCRIPTEN__
        if (glfwRawMouseMotionSupported()) glfwSetInputMode(pRaw, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
#endif

        glfw_detail::clear_error();

        return pWindow;
    }
}

std::shared_ptr<glfw_window> glfw_window::make(const std::string_view aName,
    const window_size_type aWindowSize, const vsync_mode aVsync) {
    return std::shared_ptr<glfw_window>(new glfw_window(aName, aWindowSize, aVsync));
}

glfw_window::glfw_window(const std::string_view aName, const window_size_type aWindowSize,
    const vsync_mode aVsync)
: m_pGLFWWindow(create_glfw_window(aName, aWindowSize, aVsync))
, m_pGLFWCursor(nullptr, destroy_cursor)
, m_Name(aName)
{}

void glfw_window::poll_events() {
    glfw_detail::clear_error();

    glfwPollEvents();

    glfw_detail::throw_error_if_any("poll_events");
}

void glfw_window::swap_buffers() { glfwSwapBuffers(m_pGLFWWindow.get()); }

glfw_window::window_pointer_type glfw_window::ptr_to_implementation() const { return m_pGLFWWindow; }

window_size_type glfw_window::window_size() const {
    int width = 0, height = 0;

    glfwGetFramebufferSize(m_pGLFWWindow.get(), &width, &height);

    return {width, height};
}

window_aspect_ratio_type glfw_window::aspect_ratio() const { return aspect_ratio_of(window_size()); }

std::string_view glfw_window::name() const { return m_Name; }

bool glfw_window::should_close() const { return glfwWindowShouldClose(m_pGLFWWindow.get()); }

void glfw_window::close() { glfwSetWindowShouldClose(m_pGLFWWindow.get(), GLFW_TRUE); }

void glfw_window::keep_open() { glfwSetWindowShouldClose(m_pGLFWWindow.get(), GLFW_FALSE); }

void glfw_window::set_icons(const icon_image_collection_type &aIconImages) {
    require_complete(aIconImages);

    if (aIconImages.empty()) {
        glfwSetWindowIcon(m_pGLFWWindow.get(), 0, nullptr);

        return;
    }

    std::vector<GLFWimage> glfwImages;

    glfwImages.reserve(aIconImages.size());

    for (const auto &iconImage : aIconImages) {
        GLFWimage image;
        image.width = static_cast<int>(iconImage.width_pixels);
        image.height = static_cast<int>(iconImage.height_pixels);
        image.pixels = const_cast<unsigned char *>(
            reinterpret_cast<const unsigned char *>(iconImage.data_rgba32.data()));

        glfwImages.push_back(image);
    }

    glfwSetWindowIcon(m_pGLFWWindow.get(), static_cast<int>(glfwImages.size()), glfwImages.data());
}

void glfw_window::set_cursor(const cursor_image_type &aRGBA32) {
    GLFWimage image;
    image.width = 16;
    image.height = 16;
    image.pixels = const_cast<unsigned char *>(aRGBA32.data());

    m_pGLFWCursor = cursor_pointer_type(glfwCreateCursor(&image, 0, 0), destroy_cursor);

    glfwSetCursor(m_pGLFWWindow.get(), m_pGLFWCursor.get());
}

void glfw_window::set_cursor(const standard_cursor_graphic aCursor) {
    int shape = GLFW_ARROW_CURSOR;

    switch (aCursor) {
        case standard_cursor_graphic::arrow: shape = GLFW_ARROW_CURSOR; break;
        case standard_cursor_graphic::ibeam: shape = GLFW_IBEAM_CURSOR; break;
        case standard_cursor_graphic::crosshair: shape = GLFW_CROSSHAIR_CURSOR; break;
        case standard_cursor_graphic::hand: shape = GLFW_HAND_CURSOR; break;
        case standard_cursor_graphic::horizontal_resizer: shape = GLFW_HRESIZE_CURSOR; break;
        case standard_cursor_graphic::vertical_resizer: shape = GLFW_VRESIZE_CURSOR; break;
    }

    m_pGLFWCursor = cursor_pointer_type(glfwCreateStandardCursor(shape), destroy_cursor);

    glfwSetCursor(m_pGLFWWindow.get(), m_pGLFWCursor.get());
}
