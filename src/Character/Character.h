#pragma once

#include "../Stage/Stage.h"

#include "raylib.h"
#include "imgui.h"

class Character
{
public:
    Character(const int gameWidth, const int gameHeight, const bool isPlayer1, const Stage &stage, const Camera2D &camera);
    ~Character() = default;

    void setPosition(float x, float y)
    {
        m_position.x = x;
        m_position.y = y;
    }
    Rectangle getPosition() const { return {m_position.x, m_position.y, m_size.x, m_size.y}; }

    void update(float dt);
    void fixedUpdate(float fixedDt);
    void render();

    void imGuiDebugRender();

    static constexpr float STARTING_POSITION_FACTOR_X = 4.0f;
    static constexpr float DEFAULT_CHARACTER_WIDTH = 40.0f;
    static constexpr float DEFAULT_CHARACTER_HEIGHT = 80.0f;

private:
    const bool m_isPlayer1;
    Vector2 m_position;
    Vector2 m_size;

    const Stage &m_stage;     // ref to the stage
    const Camera2D &m_camera; // ref to the raylib camera
};
