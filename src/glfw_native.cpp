// © Joseph Cameron - MIT License

#include <gdk/windowing/exception.h>
#include <gdk/windowing/glfw_window.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#if defined(_WIN32)
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__APPLE__)
#define GLFW_EXPOSE_NATIVE_COCOA
#else
#if GDK_WINDOWING_GLFW_X11
#define GLFW_EXPOSE_NATIVE_X11
#endif
#if GDK_WINDOWING_GLFW_WAYLAND
#define GLFW_EXPOSE_NATIVE_WAYLAND
#endif
#endif

#include <GLFW/glfw3native.h>

#include <string>

using namespace gdk::windowing;

native_handles glfw_window::handles() const {
    GLFWwindow *const pWindow = m_pGLFWWindow.get();

    switch (glfwGetPlatform()) {
#if defined(_WIN32)
        case GLFW_PLATFORM_WIN32:
            return win32_handles{.instance = GetModuleHandleW(nullptr), .window = glfwGetWin32Window(pWindow)};
#elif defined(__APPLE__)
        case GLFW_PLATFORM_COCOA:
            return cocoa_handles{.window = glfwGetCocoaWindow(pWindow)};
#else
#if GDK_WINDOWING_GLFW_X11
        case GLFW_PLATFORM_X11:
            return x11_handles{.display = glfwGetX11Display(), .window = glfwGetX11Window(pWindow)};
#endif
#if GDK_WINDOWING_GLFW_WAYLAND
        case GLFW_PLATFORM_WAYLAND:
            return wayland_handles{.display = glfwGetWaylandDisplay(), .surface = glfwGetWaylandWindow(pWindow)};
#endif
#endif
        default: break;
    }

    throw exception(std::string("gdk::windowing::glfw/handles: glfw runs on platform ")
        .append(std::to_string(glfwGetPlatform())).append(", which this build has no native handles for"));
}
