# gdk-windowing_glfw
The [gdk-windowing](https://github.com/jfcameron/gdk-windowing) interface, implemented over [GLFW](https://www.glfw.org/): windows with an OpenGL context, or with none for a renderer that brings its own, on Linux (X11 and Wayland), Windows and macOS, and a canvas in the browser through Emscripten.

API documentation: https://jfcameron.github.io/gdk-windowing_glfw/

## Getting Started
Install cmake 3.21 or greater, and a c++20 compatible compiler. On Linux, GLFW needs the X11 and Wayland development headers (on Debian and Ubuntu: `libx11-dev libxcursor-dev libxi-dev libxinerama-dev libxrandr-dev libxkbcommon-dev libwayland-dev wayland-protocols`).
```
# initialize the tree
git submodule update --init --recursive

# generate the script to build the library, its tests and the demo on linux using gcc
cmake --preset linux-gcc

# build them
cmake --build --preset linux-gcc

# run the tests: they open windows, so they need a display
ctest --preset linux-gcc
```
CI runs them on Linux (X11 and Wayland), FreeBSD (X11) and Windows x64 and x86, the last over Mesa's software OpenGL, which has no arm64 build. Hosted macOS runners have no GPU and GLFW asks macOS for an accelerated pixel format, so the macOS jobs only build. Without a display, run them under Xvfb: `xvfb-run -a -s "-noreset" ctest --preset linux-gcc`. `-noreset` matters: each time the last window goes, GLFW closes its X connection, and an Xvfb that resets on its last client refuses the next one.

See `CMakePresets.json` for the full list of supported platforms and compilers. See `.github/workflows/ci.yml` to see how CI runners build it on different platforms.

To use it from another project, add it with `add_subdirectory` and link `gdk::windowing_glfw`. It brings gdk-windowing and GLFW with it, unless the project already has them. Or install it (`cmake --install`, which installs gdk-windowing and GLFW beside it) and use `find_package(gdk-windowing_glfw)`.

## Usage
```cpp
#include <gdk/windowing/glfw_context.h>
#include <gdk/windowing/window.h>

using namespace gdk::windowing;

const context_ptr_type pContext = glfw_context::make();
const window_ptr_type pWindow = pContext->make_window("my game", {800, 600});

while (!pWindow->should_close()) {
    pContext->poll_events();

    // draw, into pWindow->window_size() pixels

    pWindow->swap_buffers();
}
```

The window makes its OpenGL context current and loads nothing: a renderer loads its own GL functions (gdk-graphics_webgles initialises GLEW itself). `glfw_window::ptr_to_implementation()` is the `GLFWwindow` that input and the renderer attach to; it may outlive the `glfw_window`.

A renderer that brings its own graphics API, Vulkan or WebGPU say, asks for windows with none: `glfw_context::make(glfw_window::graphics_api::none)`, or `glfw_window::make` with it last. Such a window makes no context, its `swap_buffers` does nothing, and vsync is the renderer's to apply when it presents. `glfw_window::handles()` gives the window as its system knows it, for the renderer to make its surface from: X11's display and window or Wayland's display and surface, whichever glfw runs on, Win32's instance and window, or Cocoa's `NSWindow`. It is not there in the browser.

`window_size()` is the framebuffer's size in pixels, which is what a viewport needs. On a scaled display it is larger than the window's size in screen coordinates.
