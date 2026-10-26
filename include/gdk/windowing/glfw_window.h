// © Joseph Cameron - MIT License

#ifndef GDK_WINDOWING_GLFW_WINDOW_H
#define GDK_WINDOWING_GLFW_WINDOW_H

#include <gdk/windowing/window.h>

#include <memory>
#include <string>
#include <string_view>

struct GLFWwindow;
struct GLFWcursor;

namespace gdk::windowing {
    /// \brief the glfw implementation of `gdk::windowing::window`
    class glfw_window final : public window {
    public:
        using window_pointer_type = std::shared_ptr<GLFWwindow>;
        using cursor_pointer_type = std::unique_ptr<GLFWcursor, void (*)(GLFWcursor *)>;

        //! Throws gdk::windowing::exception when the environment cannot give glfw a window and a GL context.
        [[nodiscard]] static std::shared_ptr<glfw_window> make(
            const std::string_view aName = "Window",
            const window_size_type aWindowSize = {400, 300},
            const vsync_mode aVsync = vsync_mode::adaptive);

        //! processes every glfw window's events. Throws gdk::windowing::exception for an error glfw reported meanwhile.
        static void poll_events();

        //! the glfw window, which input and the renderer attach to; it may outlive this
        [[nodiscard]] window_pointer_type ptr_to_implementation() const;

        virtual void swap_buffers() override;
        [[nodiscard]] virtual bool should_close() const override;
        [[nodiscard]] virtual window_aspect_ratio_type aspect_ratio() const override;
        [[nodiscard]] virtual std::string_view name() const override;
        virtual void set_cursor(const cursor_image_type &aRGBA32) override;
        virtual void set_cursor(const standard_cursor_graphic aCursor) override;
        virtual void set_icons(const icon_image_collection_type &aIconImages) override;
        [[nodiscard]] virtual window_size_type window_size() const override;
        virtual void close() override;
        virtual void keep_open() override;

        glfw_window(const glfw_window &) = delete;
        glfw_window &operator=(const glfw_window &) = delete;

        virtual ~glfw_window() override = default;

    private:
        glfw_window(const std::string_view aName, const window_size_type aWindowSize,
            const vsync_mode aVsync);

        window_pointer_type m_pGLFWWindow;
        cursor_pointer_type m_pGLFWCursor;
        std::string m_Name;
    };
}

#endif
