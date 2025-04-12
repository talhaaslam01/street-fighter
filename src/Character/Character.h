#pragma once

#include "../Input/input.h"
#include "../Stage/Stage.h"
#include "Animation.h"
#include "SpriteSheet.h"

#include "raylib.h"
#include "imgui.h"
#include <string>
#include <vector>

class Character
{
public:
    Character(const int gameWidth, const int gameHeight, const bool isPlayer1, const Stage &stage, const Camera2D &camera);
    ~Character() = default;

    void initialize();

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

    // Physics constants
    static constexpr float MOVE_SPEED = 100.0f;
    static constexpr float GRAVITY = 980.0f;

private:
    const bool m_isPlayer1;
    Vector2 m_position;
    Vector2 m_size;
    Vector2 m_velocity;

    const Stage &m_stage;     // ref to the stage
    const Camera2D &m_camera; // ref to the raylib camera

    CharacterState m_currentState;
    SpriteSheetManager m_spriteSheetManager;
    AnimationsManager m_animationsManager;
    Input m_input;

    Action m_currentAnimation;
    int m_currentAnimFrame;
    float m_animTimer;
    int m_facingDirection; // 1 right, -1 left
};
