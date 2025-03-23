#include "Character.h"
#include "raymath.h"

Character::Character(const int gameWidth, const int gameHeight, const bool isPlayer1,
                     const Stage &stage, const Camera2D &camera)
    : m_isPlayer1(isPlayer1),
      m_position({}),
      m_size({DEFAULT_CHARACTER_WIDTH, DEFAULT_CHARACTER_HEIGHT}),
      m_stage(stage),
      m_camera(camera)
{
    // set initial character position
    m_position.y = m_stage.getFloorY();

    // X position depends on player number
    float xPosScreenCoord;
    if (m_isPlayer1)
    {
        xPosScreenCoord = gameWidth / STARTING_POSITION_FACTOR_X;
    }
    else
    {
        xPosScreenCoord = gameWidth - m_size.x - (gameWidth / STARTING_POSITION_FACTOR_X);
    }
    m_position.x = GetScreenToWorld2D({xPosScreenCoord, 0.0f}, m_camera).x;
}

void Character::update(float dt)
{
    // Update character logic
}

void Character::fixedUpdate(float fixedDt)
{
    // Update character physics
}

void Character::render()
{

    // Debug render
    Color color = m_isPlayer1 ? BLUE : MAGENTA;
    DrawRectangle(
        static_cast<int>(m_position.x),
        static_cast<int>(m_position.y - m_size.y),
        static_cast<int>(m_size.x),
        static_cast<int>(m_size.y),
        color);
}

void Character::imGuiDebugRender()
{
    const char *name = m_isPlayer1 ? "Player 1" : "Player 2";

    ImGui::Begin(name);
    {
        ImGui::SliderFloat("Pos x", &m_position.x, -1000.0f, 1000.0f);
        ImGui::SliderFloat("Pos y", &m_position.y, -1000.0f, 1000.0f);
    }
    ImGui::End();
}
