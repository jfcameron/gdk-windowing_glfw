// © Joseph Cameron - MIT License

#include <gdk/windowing/glfw_context.h>

#include <gdk/windowing/glfw_window.h>
#include <gdk/windowing/impl_glfw_session.h>

using namespace gdk::windowing;

glfw_context::glfw_context()
: m_pSession(glfw_detail::acquire_session())
{}

std::shared_ptr<glfw_context> glfw_context::make() {
    return std::shared_ptr<glfw_context>(new glfw_context());
}

window_ptr_type glfw_context::make_window(const std::string_view aName,
    const window_size_type aWindowSize, const vsync_mode aVsync) {
    return glfw_window::make(aName, aWindowSize, aVsync);
}

void glfw_context::poll_events() { glfw_window::poll_events(); }
