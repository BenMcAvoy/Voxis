#include "camera.hpp"

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

void Camera::update(float aspectRatio)
{
    const bx::Vec3 at = {pos.x, pos.y, pos.z - 1.0f};
    const bx::Vec3 up = {0.0f, 1.0f, 0.0f};
    bx::mtxLookAt(view, pos, at, up, bx::Handedness::Right);

    const bgfx::Caps *caps = bgfx::getCaps();
    bx::mtxProj(projection, bx::toDeg(fov), aspectRatio, near_z, far_z, caps->homogeneousDepth, bx::Handedness::Right);
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