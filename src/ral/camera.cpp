#include "camera.hpp"
#include "window.hpp"

using namespace vox::ral;

Camera::Camera(float aspectRatio)
{
    spdlog::trace("Creating camera");
    update(aspectRatio);
}

Camera::~Camera()
{
    spdlog::trace("Destroying camera");
}

void Camera::update(float aspectRatio, Window *window)
{
    const float yawRad = bx::toRad(yaw);
    const float pitchRad = bx::toRad(pitch);
    const bx::Vec3 forward = {
        bx::cos(pitchRad) * bx::sin(yawRad),
        bx::sin(pitchRad),
        bx::cos(pitchRad) * bx::cos(yawRad)};
    const bx::Vec3 at = bx::add(pos, forward);
    const bx::Vec3 up = {0.0f, 1.0f, 0.0f};
    bx::mtxLookAt(view, pos, at, up, bx::Handedness::Right);

    const bgfx::Caps *caps = bgfx::getCaps();
    bx::mtxProj(projection, bx::toDeg(fov), aspectRatio, near_z, far_z, caps->homogeneousDepth, bx::Handedness::Right);

    if (window)
    {
        // WASD and mouse input
        const float moveSpeed = 0.2f;        // units per second
        const float mouseSensitivity = 0.1f; // degrees per pixel

        // Horizontal forward/right vectors (yaw only, ignore pitch like Minecraft)
        const bx::Vec3 hForward = {bx::sin(yawRad), 0.0f, bx::cos(yawRad)};
        const bx::Vec3 hRight = {-bx::cos(yawRad), 0.0f, bx::sin(yawRad)};

        const float dt = moveSpeed * 0.016f;
        if (window->keyStates[SDL_SCANCODE_W] == KeyState::Pressed || window->keyStates[SDL_SCANCODE_W] == KeyState::Held)
            pos = bx::add(pos, bx::mul(hForward, dt));
        if (window->keyStates[SDL_SCANCODE_S] == KeyState::Pressed || window->keyStates[SDL_SCANCODE_S] == KeyState::Held)
            pos = bx::add(pos, bx::mul(hForward, -dt));
        if (window->keyStates[SDL_SCANCODE_A] == KeyState::Pressed || window->keyStates[SDL_SCANCODE_A] == KeyState::Held)
            pos = bx::add(pos, bx::mul(hRight, -dt));
        if (window->keyStates[SDL_SCANCODE_D] == KeyState::Pressed || window->keyStates[SDL_SCANCODE_D] == KeyState::Held)
            pos = bx::add(pos, bx::mul(hRight, dt));
        if (window->keyStates[SDL_SCANCODE_SPACE] == KeyState::Pressed || window->keyStates[SDL_SCANCODE_SPACE] == KeyState::Held)
            pos.y += dt;
        if (window->keyStates[SDL_SCANCODE_LSHIFT] == KeyState::Pressed || window->keyStates[SDL_SCANCODE_LSHIFT] == KeyState::Held)
            pos.y -= dt;

        yaw -= window->mouseDeltaX * mouseSensitivity;
        pitch -= window->mouseDeltaY * mouseSensitivity;

        // Clamp pitch to avoid flipping
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }
}

void Camera::submit(bgfx::ViewId viewId) const
{
    bgfx::setViewTransform(viewId, view, projection);
}

void Camera::setPosition(float x, float y, float z)
{
    pos = {x, y, z};
}

void Camera::setRotation(float yawDegrees, float pitchDegrees)
{
    yaw = bx::toRad(yawDegrees);
    pitch = bx::toRad(pitchDegrees);
}

void Camera::setFOV(float fovDegrees)
{
    fov = bx::toRad(fovDegrees);
}

void Camera::setNearFar(float nearZ, float farZ)
{
    near_z = nearZ;
    far_z = farZ;
}