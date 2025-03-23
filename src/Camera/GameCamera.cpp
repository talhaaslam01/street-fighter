#include "GameCamera.h"
#include "raymath.h"
#include "imgui.h"

GameCamera::GameCamera(const int gameWidth, const int gameHeight, const Vector4 &stageBounds)
    : m_camera({}),
      m_target({0.0f, 0.0f}),
      m_followSpeed(12.5f),
      m_cameraBounds({0.0f, 0.0f, 0.0f, 0.0f}),
      m_xCameraMovementBound({gameWidth / CAMERA_MOVEMENT_THRESHOLD_FACTOR,
                              gameWidth - (gameWidth / CAMERA_MOVEMENT_THRESHOLD_FACTOR)})

{
    // Initialize raylib camera
    m_camera.offset = {gameWidth / 2.0f, gameHeight / 2.0f};
    m_camera.rotation = 0.0f;
    m_camera.zoom = 1.0f;

    // set camera bounds
    m_cameraBounds.x = stageBounds.x + m_camera.offset.x;
    m_cameraBounds.y = stageBounds.y + m_camera.offset.y;
    m_cameraBounds.z = stageBounds.z + m_camera.offset.x - gameWidth;
    m_cameraBounds.w = stageBounds.w + m_camera.offset.y - gameHeight;

    // Set camera target to center of the stage
    const float stageWidth = stageBounds.z - stageBounds.x;
    const float stageHeight = stageBounds.w - stageBounds.y;

    const float stageCenterX = (stageWidth + m_camera.offset.x) / 2.0f;
    const float stageCenterY = (stageHeight + m_camera.offset.y) / 2.0f;

    m_camera.target.x = Clamp(stageCenterX, m_cameraBounds.x, m_cameraBounds.z);
    m_camera.target.y = Clamp(stageCenterY, m_cameraBounds.y, m_cameraBounds.w);
    m_target = m_camera.target;
}

void GameCamera::update(const Rectangle p1, const Rectangle p2, float dt)
{
    // for verticle movement simply follow the player who is closest to the top
    // TODO: adjust this 20 later for
    m_target.y = fmin(p1.y, p2.y) - 20.0f;
    m_target.y = Clamp(m_target.y, m_cameraBounds.y, m_cameraBounds.w);
    // Smoothly move camera towards target Y position
    if (fabs(m_camera.target.y - m_target.y) < 0.8f)
    {
        m_camera.target.y = m_target.y;
    }
    else
    {
        m_camera.target.y = Lerp(m_camera.target.y, m_target.y, m_followSpeed * dt);
    }

    // Deadzone for horizontal movement
    // Convert player positions from world space to screen space
    Vector2 playerOnePos = GetWorldToScreen2D({p1.x, p1.y}, m_camera);
    Vector2 playerTwoPos = GetWorldToScreen2D({p2.x, p2.y}, m_camera);

    // Include player widths to get their right edges in screen space
    float playerOneRight = playerOnePos.x + p1.width;
    float playerTwoRight = playerTwoPos.x + p2.width;

    // Find leftmost and rightmost positions of both players
    float leftmostX = fmin(playerOnePos.x, playerTwoPos.x);
    float rightmostX = fmax(playerOneRight, playerTwoRight);

    // Calculate width between the leftmost and rightmost points of both players
    float playerSpan = rightmostX - leftmostX;

    // Get the available screen width between deadzone boundaries
    float screenWidth = m_xCameraMovementBound.y - m_xCameraMovementBound.x;

    float targetShiftX = 0.0f;

    // Case 1: Players are too far apart (both outside deadzones)
    if (playerSpan > screenWidth)
    {
        // Center the camera on the midpoint between players
        float midpoint = leftmostX + playerSpan / 2.0f;
        float screenCenter = (m_xCameraMovementBound.x + m_xCameraMovementBound.y) / 2.0f;
        targetShiftX = midpoint - screenCenter;
    }
    // Case 2: One or both players are outside their respective deadzone
    else
    {
        // If leftmost player is beyond left deadzone boundary
        if (leftmostX < m_xCameraMovementBound.x)
        {
            targetShiftX = leftmostX - m_xCameraMovementBound.x;
        }
        // If rightmost player is beyond right deadzone boundary
        else if (rightmostX > m_xCameraMovementBound.y)
        {
            targetShiftX = rightmostX - m_xCameraMovementBound.y;
        }
    }

    // Only move camera if there's a significant shift needed
    if (fabs(targetShiftX) > 0.5f)
    {
        // Convert screen space offset to world space
        float worldShiftX = targetShiftX / m_camera.zoom;

        // Apply the offset to the camera target
        m_target.x += worldShiftX;

        // Ensure camera doesn't scroll past stage boundaries
        m_target.x = Clamp(m_target.x, m_cameraBounds.x, m_cameraBounds.z);
    }

    // Smoothly move camera towards target X position
    if (fabs(m_camera.target.x - m_target.x) < 0.8f)
    {
        m_camera.target.x = m_target.x;
    }
    else
    {
        m_camera.target.x = Lerp(m_camera.target.x, m_target.x, m_followSpeed * dt);
    }
}

void GameCamera::render()
{
    // Draw camera target
    DrawCircle(static_cast<int>(m_camera.target.x), static_cast<int>(m_camera.target.y), 1, MAGENTA);
    DrawCircle(static_cast<int>(m_target.x), static_cast<int>(m_target.y), 1, WHITE);

    EndMode2D();
    {
        // threshold for camera follow
        DrawLine(m_xCameraMovementBound.x, 0, m_xCameraMovementBound.x, GetScreenHeight(), YELLOW);
        DrawLine(m_xCameraMovementBound.y, 0, m_xCameraMovementBound.y, GetScreenHeight(), YELLOW);
    }
    BeginMode2D(m_camera);
}

// imgui debug
void GameCamera::imguiDebugRender()
{
    ImGui::Begin("Camera Debug");
    {
        ImGui::Text("Offset X: %f, Y: %f", m_camera.offset.x, m_camera.offset.y);
        ImGui::Text("Cmaera bounds: left: %f, right: %f", m_cameraBounds.x, m_cameraBounds.z);
        ImGui::Text("Bounds: top: %f, bottom: %f", m_cameraBounds.y, m_cameraBounds.w);
        ImGui::SliderFloat("Follow Speed", &m_followSpeed, 0.0f, 25.0f);
        ImGui::SliderFloat2("Camera target", (float *)&m_camera.target, -1000.0f, 1000.0f);
        ImGui::SliderFloat2("Target", (float *)&m_target, -1000.0f, 1000.0f);
    }
    ImGui::End();
}
