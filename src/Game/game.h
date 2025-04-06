#pragma once

#include "../Camera/GameCamera.h"
#include "../Character/Character.h"
#include "../Stage/Stage.h"

#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"

class Game
{
public:
    Game();
    ~Game();

    // Main game loop methods
    void initialize();
    void run();
    void shutdown();

    // Time management
    float getDeltaTime() const { return m_deltaTime; }
    float getFixedDeltaTime() const { return 1.0f / m_targetFPS; }
    double getTotalTime() const { return m_totalTime; }

    // Constants
    static const char *WINDOW_TITLE;
    static const int GAME_WIDTH;
    static const int GAME_HEIGHT;
    static const int GAME_SCALE_FACTOR;
    static const int TARGET_FPS;

private:
    void updateScalingRects();

    // Core loop functions
    void processInput();
    void update(float dt);
    void fixedUpdate(float fixedDt);
    void render(const float alpha);
    void imGuiDebugRender();

    // Core properties
    int m_targetFPS;
    bool m_isRunning;
    int m_windowWidth;
    int m_windowHeight;
    const char *m_windowTitle;

    // Render texture and scaling rectangles
    RenderTexture2D m_gameTexture{};
    Rectangle m_sourceRect{};
    Rectangle m_destRect{};

    // Time tracking
    float m_deltaTime;
    double m_totalTime;
    double m_accumulator;

    // entities
    Stage m_stage;
    GameCamera m_camera;
    Character m_player1;
    Character m_player2;

    // frame by frame debugging
    int m_isPaused;
    int m_stepFrame;
};
