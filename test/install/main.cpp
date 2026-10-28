// © Joseph Cameron - MIT License

#include <gdk/windowing/glfw_context.h>

int main() {
    const auto pContext = gdk::windowing::glfw_context::make();

    pContext->poll_events();

    return 0;
}
