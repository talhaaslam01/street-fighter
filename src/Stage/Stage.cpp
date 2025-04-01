#include "Stage.h"

#include "imgui.h"

Stage::Stage(const std::string &name)
    : m_name(name),
      m_bounds({112.0f, 0.0f, 768.0f, 240.0f}),
      m_floorY(210.0f)
{
}

Stage::~Stage()
{
    unload();
}

void Stage::load()
{
    // TODO: refactor later to allow more stages
    m_bounds = {112.0f, 0.0f, 768.0f, 240.0f}; // Stage boundary x1, y1, x2, y2
    m_floorY = 210.0f;                         // Position of the floor

    BackgroundLayer stage;
    stage.texture = LoadTexture("resources/stages/suzakuCastle.png");
    stage.position = {0.0f, 0.0f};
    stage.parallaxFactor = 1.0f;

    m_backgroundLayers.push_back(stage);
}

void Stage::unload()
{
    // Unload all textures
    for (auto &layer : m_backgroundLayers)
    {
        UnloadTexture(layer.texture);
    }
    m_backgroundLayers.clear();
}

void Stage::update(float dt)
{
    // Update animation timer
    // m_animTimer += dt;

    // Any animation updates for the background elements
    // For example, waving flags, water ripples, etc.
}

void Stage::render(const Camera2D &camera)
{
    // Render all background layers with parallax scrolling
    for (const auto &layer : m_backgroundLayers)
    {
        // Apply parallax by adjusting position
        // TODO: may need to fix this later when adding more elements
        float parallaxX = layer.position.x * layer.parallaxFactor +
                          (camera.target.x * (1 - layer.parallaxFactor));

        // Draw the layer
        DrawTexture(layer.texture, static_cast<int>(parallaxX), static_cast<int>(layer.position.y), WHITE);
    }

    // Left boundary
    DrawLine(
        static_cast<int>(m_bounds.x),
        static_cast<int>(m_bounds.y),
        static_cast<int>(m_bounds.x),
        static_cast<int>(m_bounds.w),
        RED);

    // Right boundary
    DrawLine(
        static_cast<int>(m_bounds.z),
        static_cast<int>(m_bounds.y),
        static_cast<int>(m_bounds.z),
        static_cast<int>(m_bounds.w),
        RED);

    // Top boundary
    DrawLine(
        static_cast<int>(m_bounds.x),
        static_cast<int>(m_bounds.y),
        static_cast<int>(m_bounds.z),
        static_cast<int>(m_bounds.y),
        RED);

    // Bottom boundary
    DrawLine(
        static_cast<int>(m_bounds.x),
        static_cast<int>(m_bounds.w),
        static_cast<int>(m_bounds.z),
        static_cast<int>(m_bounds.w),
        RED);

    // Draw the floor
    DrawLine(
        0, static_cast<int>(m_floorY),
        GetScreenWidth(), static_cast<int>(m_floorY),
        GREEN);
}

void Stage::imguiDebugRender(GameCamera &gameCamera)
{
    // ImGui::Begin("Stage Debug");
    // {
    //     // Controls for stage boundaries
    //     if (ImGui::SliderFloat("Left Boundary", &m_bounds.x, 0.0f, m_bounds.z - 1.0f))
    //     {
    //         // gameCamera.setm_bounds(m_leftBoundary, m_rightBoundary);
    //     }
    //     if (ImGui::SliderFloat("Right Boundary", &m_bounds.z, m_bounds.x + 1.0f, 1000.0f))
    //     {
    //         // gameCamera.setm_bounds(m_leftBoundary, m_rightBoundary);
    //     }

    //     ImGui::SliderFloat("Floor Y", &m_floorY, 0.0f, 500.0f);

    //     auto camera = gameCamera.getCamera();
    //     auto SC = GetWorldToScreen2D({m_bounds.x, 0}, camera);
    //     ImGui::Text("Test: %f, %f", m_bounds.x, SC.x);
    //     ImGui::Text("Left bound WC: %f, SC: %f", m_bounds.x, m_bounds.x + camera.offset.x - camera.target.x);
    //     ImGui::Text("Right bound WC: %f, SC: %f", m_bounds.z, m_bounds.z + camera.offset.x - camera.target.x);
    //     // ImGui::Text("Top bound WC: %f, SC: %f", m_bounds.y, m_bounds.y - gameCamera.getCamera().target.y);
    //     // ImGui::Text("Bottom bound WC: %f, SC: %f", m_bounds.w, m_bounds.w - gameCamera.getCamera().target.y);
    //     // ImGui::Text("Floor Y WC: %f, SC: %f", m_floorY, m_floorY - gameCamera.getCamera().target.y);
    // }
    // ImGui::End();
}
