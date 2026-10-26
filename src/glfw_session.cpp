// © Joseph Cameron - MIT License

#include <gdk/windowing/impl_glfw_session.h>

#include <gdk/windowing/exception.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstdio>

using namespace gdk::windowing;
using namespace gdk::windowing::glfw_detail;

namespace {
    char gLastError[256] = {};

    bool gHasError = false;

    void record_error(int, const char *const aMessage) noexcept {
        std::snprintf(gLastError, sizeof gLastError, "%s",
            aMessage ? aMessage : "glfw reported an error it could not describe");

        gHasError = true;
    }
}

void glfw_detail::clear_error() noexcept {
    gLastError[0] = '\0';

    gHasError = false;
}

std::optional<std::string> glfw_detail::take_error() {
    if (!gHasError) return {};

    std::string out(gLastError);

    clear_error();

    return out;
}

void glfw_detail::throw_error_if_any(const char *const aWhere) {
    if (const auto error = take_error())
        throw exception(std::string("gdk::windowing::glfw/").append(aWhere).append(": ").append(*error));
}

session::session() {
    glfwSetErrorCallback(record_error);

    clear_error();

    if (!glfwInit())
        throw exception(std::string("gdk::windowing::glfw/glfwInit failed: ")
            .append(take_error().value_or("glfw gave no reason")));

    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
}

session::~session() { glfwTerminate(); }

GLFWwindow *session::share_root() const { return mLive.empty() ? nullptr : mLive.front(); }

void session::joined(GLFWwindow *const apWindow) { mLive.push_back(apWindow); }

void session::left(GLFWwindow *const apWindow) {
    mLive.erase(std::remove(mLive.begin(), mLive.end(), apWindow), mLive.end());
}

std::shared_ptr<session> glfw_detail::acquire_session() {
    static std::weak_ptr<session> weak;

    auto shared = weak.lock();

    if (!shared) {
        shared = std::make_shared<session>();

        weak = shared;
    }

    return shared;
}
