#pragma once

#include <spdlog/spdlog.h>

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

#include <bgfx/bgfx.h>

#include <string_view>

namespace vox::ral
{
    class Window
    {
    public:
        Window(std::string_view title, int width, int height, bgfx::RendererType::Enum rendererType = bgfx::RendererType::Count);
        ~Window();

        SDL_Window *getSDLWindow() const { return window; }

        // poll
        bool pollEvents();
        bool begin();
        void end() const;
        bool nextFrame();

        void clear() const;
        void present() const;

        float getAspectRatio() const;

    private:
        SDL_Window *window{};
        int width, height;

#ifndef _NDEBUG
        struct BgfxCallbacks : bgfx::CallbackI
        {
            void fatal(const char *filePath, uint16_t line, bgfx::Fatal::Enum code, const char *str) override
            {
                spdlog::critical("[bgfx] {}:{} (code {}): {}", filePath, line, static_cast<int>(code), str);
            }

            void traceVargs(const char *filePath, uint16_t line, const char *format, va_list argList) override
            {
                char buf[2048];
                vsnprintf(buf, sizeof(buf), format, argList);

                std::string_view msg(buf);
                while (!msg.empty() && (msg.back() == '\n' || msg.back() == '\r'))
                    msg.remove_suffix(1);
                const bool isNoise = msg.contains("device feature level") || msg.contains("RefCount is");
                if (msg.contains("LEAK"))
                    spdlog::error("[bgfx] {}:{}: {}", filePath, line, msg);
                else if (msg.contains("WARN") && !isNoise)
                    spdlog::warn("[bgfx] {}:{}: {}", filePath, line, msg);
                else
                    spdlog::trace("[bgfx] {}:{}: {}", filePath, line, msg);
            }

            void profilerBegin(const char *, uint32_t, const char *, uint16_t) override {}
            void profilerBeginLiteral(const char *, uint32_t, const char *, uint16_t) override {}
            void profilerEnd() override {}

            uint32_t cacheReadSize(uint64_t) override { return 0; }
            bool cacheRead(uint64_t, void *, uint32_t) override { return false; }
            void cacheWrite(uint64_t, const void *, uint32_t) override {}

            void screenShot(const char *, uint32_t, uint32_t, uint32_t, const void *, uint32_t, bool) override {}
            void captureBegin(uint32_t, uint32_t, uint32_t, bgfx::TextureFormat::Enum, bool) override {}
            void captureEnd() override {}
            void captureFrame(const void *, uint32_t) override {}
        };
        BgfxCallbacks bgfxCallbacks{};
#endif
    };
} // namespace vox::ral