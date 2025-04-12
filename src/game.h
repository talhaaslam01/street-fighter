#pragma once

#include "Camera/GameCamera.h"
#include "Character/Character.h"
#include "Stage/Stage.h"
#include "raylib.h"

class Game
{
public:
    Game();
    ~Game();

    // Main game loop methods
    void initialize();
    void run();
    void shutdown();

private:
    void updateScalingRects();

    // Core loop functions
    void processInput();
    void update(const float dt);
    void fixedUpdate(const float fixedDt);
    void render(const float alpha);
    void imGuiDebugRender();

    // Core properties
    bool m_isRunning;
    int m_windowWidth;
    int m_windowHeight;

    // Render texture and scaling rectangles
    RenderTexture2D m_gameTexture{};
    Rectangle m_sourceRect{};
    Rectangle m_destRect{};

    // entities
    Stage m_stage;
    GameCamera m_camera;
    Character m_player1;
    Character m_player2;

    // frame by frame debugging
    int m_isPaused;
    int m_stepFrame;
};
