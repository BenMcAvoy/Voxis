#include "window.hpp"

using namespace vox::ral;

Window::Window(std::string_view title, int width, int height, bgfx::RendererType::Enum rendererType) : width(width), height(height)
{
    spdlog::trace("Creating window '{}' with size {}x{}", title, width, height);

    window = SDL_CreateWindow(title.data(), width, height, 0);
    if (!window)
    {
        spdlog::error("Failed to create SDL window: {}", SDL_GetError());
        throw std::runtime_error("Failed to create SDL window");
    }

    spdlog::info("Created SDL window '{}'", title);

    // TODO: Support non-Windows platforms
    bgfx::Init init;

#ifdef _WIN32
    init.platformData.nwh = SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
#elif defined(__linux__)
    init.platformData.nwh = SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_X11_WINDOW_POINTER, nullptr);
#elif defined(__APPLE__)
    init.platformData.nwh = SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
#elif defined(__EMSCRIPTEN__)
    init.platformData.nwh = SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_EMSCRIPTEN_WEBGL_CONTEXT_POINTER, nullptr);
#else
#error "Unsupported platform for window handle retrieval"
#endif

    init.resolution.width = static_cast<uint32_t>(width);
    init.resolution.height = static_cast<uint32_t>(height);

    init.type = rendererType;

#ifndef _NDEBUG
    init.callback = &bgfxCallbacks;
#endif

    if (!bgfx::init(init))
    {
        spdlog::error("Failed to initialize BGFX");
        SDL_DestroyWindow(window);
        throw std::runtime_error("Failed to initialize BGFX");
    }

    // log which backend was selected
    switch (bgfx::getRendererType())
    {
    case bgfx::RendererType::Direct3D11:
        spdlog::info("Selected Direct3D 11 renderer");
        break;
    case bgfx::RendererType::Direct3D12:
        spdlog::info("Selected Direct3D 12 renderer");
        break;
    case bgfx::RendererType::OpenGL:
        spdlog::info("Selected OpenGL renderer");
        break;
    case bgfx::RendererType::OpenGLES:
        spdlog::info("Selected OpenGL ES renderer");
        break;
    case bgfx::RendererType::Vulkan:
        spdlog::info("Selected Vulkan renderer");
        break;
    default:
        spdlog::warn("Selected unknown renderer type");
        break;
    }

    spdlog::info("BGFX initialized successfully");
}

Window::~Window()
{
    bgfx::shutdown();
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Window::pollEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
            return false;
        else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
            return false;
        else if (event.type == SDL_EVENT_WINDOW_RESIZED)
        {
            width = event.window.data1;
            height = event.window.data2;

            spdlog::info("Window resized to {}x{}", width, height);
        }
    }
    return true;
}

void Window::clear() const
{
    bgfx::setViewRect(0, 0, 0, width, height);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL, 0x202020ff, 1.0f, 0);
}

void Window::present() const
{
    bgfx::frame();
}

bool Window::begin()
{
    clear();
    return true;
}

void Window::end() const
{
    present();
}

bool Window::nextFrame()
{
    static bool presentNeeded = false;
    if (presentNeeded)
        present();
    else
        presentNeeded = true;

    bool res = pollEvents();
    clear();
    return res;
}

float Window::getAspectRatio() const
{
    return static_cast<float>(width) / static_cast<float>(height);
}