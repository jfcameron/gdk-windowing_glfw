// © Joseph Cameron - MIT License

#ifndef GDK_WINDOWING_GLFW_WINDOW_H
#define GDK_WINDOWING_GLFW_WINDOW_H

#include <gdk/windowing/window.h>

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <variant>

struct GLFWwindow;
struct GLFWcursor;

namespace gdk::windowing {
#ifndef __EMSCRIPTEN__
    //! X11's `Display *` and `Window`
    struct x11_handles final {
        void *display = nullptr;
        std::uint64_t window = 0;

        [[nodiscard]] bool operator==(const x11_handles &) const = default;
    };

    //! Wayland's `wl_display *` and `wl_surface *`
    struct wayland_handles final {
        void *display = nullptr;
        void *surface = nullptr;

        [[nodiscard]] bool operator==(const wayland_handles &) const = default;
    };

    //! Win32's `HINSTANCE` and `HWND`
    struct win32_handles final {
        void *instance = nullptr;
        void *window = nullptr;

        [[nodiscard]] bool operator==(const win32_handles &) const = default;
    };

    //! Cocoa's `NSWindow *`
    struct cocoa_handles final {
        void *window = nullptr;

        [[nodiscard]] bool operator==(const cocoa_handles &) const = default;
    };

    using native_handles = std::variant<x11_handles, wayland_handles, win32_handles, cocoa_handles>;
#endif

    /// \brief the glfw implementation of `gdk::windowing::window`
    class glfw_window final : public window {
    public:
        using window_pointer_type = std::shared_ptr<GLFWwindow>;
        using cursor_pointer_type = std::unique_ptr<GLFWcursor, void (*)(GLFWcursor *)>;

        /// \brief what a window is drawn with: an OpenGL context, which it makes current, or none, for a renderer
        /// that makes its own surface from the window's native handles and presents itself
        enum class graphics_api {
            opengl,
            none
        };

        /// \brief vsync is applied to the OpenGL context; a window with none leaves it to the renderer
        /// \throws gdk::windowing::exception when the environment cannot give glfw a window, and a GL context
        /// where one is asked for
        [[nodiscard]] static std::shared_ptr<glfw_window> make(
            const std::string_view aName = "Window",
            const window_size_type aWindowSize = {400, 300},
            const vsync_mode aVsync = vsync_mode::adaptive,
            const graphics_api aGraphics = graphics_api::opengl);

        //! processes every glfw window's events. Throws gdk::windowing::exception for an error glfw reported meanwhile.
        static void poll_events();

        //! the glfw window, which input and the renderer attach to; it may outlive this
        [[nodiscard]] window_pointer_type ptr_to_implementation() const;

        [[nodiscard]] graphics_api graphics() const;

#ifndef __EMSCRIPTEN__
        /// \brief the window as its system knows it, which a renderer makes a surface from: X11's or Wayland's,
        /// whichever glfw runs on, Win32's, or Cocoa's
        /// \throws gdk::windowing::exception where glfw runs on no window system, or one it was built without
        [[nodiscard]] native_handles handles() const;
#endif

        //! does nothing for a window with no graphics API, whose renderer presents
        virtual void swap_buffers() override;
        [[nodiscard]] virtual bool should_close() const override;
        [[nodiscard]] virtual window_aspect_ratio_type aspect_ratio() const override;
        [[nodiscard]] virtual std::string_view name() const override;
        virtual void set_cursor(const cursor_image_type &aRGBA32) override;
        virtual void set_cursor(const standard_cursor_graphic aCursor) override;
        virtual void set_icons(const icon_image_collection_type &aIconImages) override;
        virtual void set_fullscreen(const bool aFullscreen) override;
        [[nodiscard]] virtual bool fullscreen() const override;
        [[nodiscard]] virtual window_size_type window_size() const override;
        virtual void close() override;
        virtual void keep_open() override;

        glfw_window(const glfw_window &) = delete;
        glfw_window &operator=(const glfw_window &) = delete;

        virtual ~glfw_window() override = default;

    private:
        glfw_window(const std::string_view aName, const window_size_type aWindowSize,
            const vsync_mode aVsync, const graphics_api aGraphics);

        window_pointer_type m_pGLFWWindow;
        cursor_pointer_type m_pGLFWCursor;
        std::string m_Name;
        graphics_api m_Graphics = graphics_api::opengl;

        //! where the window goes back to from fullscreen
        struct {
            int x = 0, y = 0, width = 0, height = 0;
        } m_Windowed;
    };
}

#endif
