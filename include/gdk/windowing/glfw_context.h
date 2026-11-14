// © Joseph Cameron - MIT License

#ifndef GDK_WINDOWING_GLFW_CONTEXT_H
#define GDK_WINDOWING_GLFW_CONTEXT_H

#include <gdk/windowing/context.h>
#include <gdk/windowing/glfw_window.h>

#include <memory>
#include <string_view>

namespace gdk::windowing {
    namespace glfw_detail { class session; }

    /// \brief the glfw implementation of `gdk::windowing::context`. glfw stays initialised while it lives
    class glfw_context final : public context {
    public:
        /// \brief a context whose windows are drawn with aGraphics
        /// \throws gdk::windowing::exception when glfw cannot start
        [[nodiscard]] static std::shared_ptr<glfw_context> make(
            const glfw_window::graphics_api aGraphics = glfw_window::graphics_api::opengl);

        [[nodiscard]] virtual window_ptr_type make_window(
            const std::string_view aName = "Window",
            const window_size_type aWindowSize = {400, 300},
            const vsync_mode aVsync = vsync_mode::adaptive) override;

        virtual void poll_events() override;

        virtual ~glfw_context() override = default;

    private:
        explicit glfw_context(const glfw_window::graphics_api aGraphics);

        std::shared_ptr<glfw_detail::session> m_pSession;
        glfw_window::graphics_api m_Graphics = glfw_window::graphics_api::opengl;
    };
}

#endif
