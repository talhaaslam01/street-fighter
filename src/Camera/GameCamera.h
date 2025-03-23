#pragma once

#include "raylib.h"

class GameCamera
{
public:
    GameCamera(const int gameWidth, const int gameHeight, const Vector4 &stageBounds);
    ~GameCamera() = default;

    void update(const Rectangle p1, const Rectangle p2, float dt);
    void render();

    // Get raylib camera for rendering
    const Camera2D &getCamera() const { return m_camera; }

    // imgui debug
    void imguiDebugRender();

    static constexpr float CAMERA_MOVEMENT_THRESHOLD_FACTOR = 7.0f;

private:
    Camera2D m_camera;              // Raylib's camera struct
    Vector2 m_target;               // Camera target position
    float m_followSpeed;            // How quickly camera follows players
    Vector4 m_cameraBounds;         // Camera bounds (left, top, right, bottom)
    Vector2 m_xCameraMovementBound; // x = left, y = right, in Screen Coordinates
};