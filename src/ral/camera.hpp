#pragma once

#include <bgfx/bgfx.h>
#include <bx/math.h>

#include <spdlog/spdlog.h>

namespace vox::ral
{
    class Window;

    class Camera
    {
    public:
        Camera(float aspectRatio = 1.0f);
        ~Camera();

        void update(float aspectRatio, Window *window = nullptr);
        void submit(bgfx::ViewId view) const;

        void setPosition(float x, float y, float z);
        void setRotation(float yawDegrees, float pitchDegrees);
        void setFOV(float fovDegrees);
        void setNearFar(float nearZ, float farZ);

    private:
        bx::Vec3 pos{0.0f, 0.0f, 2.0f};
        float yaw = 180.0f;
        float pitch = 0.0f;

        float fov = bx::toRad(90.0f);
        float near_z = 0.1f;
        float far_z = 1000.0f;

        float view[16];
        float projection[16];
    };
}
