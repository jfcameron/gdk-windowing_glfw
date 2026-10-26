// © Joseph Cameron - MIT License

#include "conformance.h"

#include <gdk/windowing/glfw_context.h>

namespace gdk::windowing::conformance {
    context_ptr_type make_context() { return glfw_context::make(); }

    std::string_view implementation() { return "glfw"; }
}
