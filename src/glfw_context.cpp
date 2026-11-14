// © Joseph Cameron - MIT License

#include <gdk/windowing/glfw_context.h>

#include <gdk/windowing/glfw_window.h>
#include <gdk/windowing/impl_glfw_session.h>

using namespace gdk::windowing;

glfw_context::glfw_context(const glfw_window::graphics_api aGraphics)
: m_pSession(glfw_detail::acquire_session())
, m_Graphics(aGraphics)
{}

std::shared_ptr<glfw_context> glfw_context::make(const glfw_window::graphics_api aGraphics) {
    return std::shared_ptr<glfw_context>(new glfw_context(aGraphics));
}

window_ptr_type glfw_context::make_window(const std::string_view aName,
    const window_size_type aWindowSize, const vsync_mode aVsync) {
    return glfw_window::make(aName, aWindowSize, aVsync, m_Graphics);
}

void glfw_context::poll_events() { glfw_window::poll_events(); }
