// © Joseph Cameron - MIT License

#ifndef GDK_WINDOWING_IMPL_GLFW_SESSION_H
#define GDK_WINDOWING_IMPL_GLFW_SESSION_H

#include <memory>
#include <optional>
#include <string>
#include <vector>

struct GLFWwindow;

namespace gdk::windowing::glfw_detail {
    class session final {
    public:
        session();
        ~session();

        session(const session &) = delete;
        session &operator=(const session &) = delete;

        [[nodiscard]] GLFWwindow *share_root() const;

        void joined(GLFWwindow *const apWindow);
        void left(GLFWwindow *const apWindow);

    private:
        std::vector<GLFWwindow *> mLive;
    };

    [[nodiscard]] std::shared_ptr<session> acquire_session();

    void clear_error() noexcept;

    [[nodiscard]] std::optional<std::string> take_error();

    void throw_error_if_any(const char *const aWhere);
}

#endif
