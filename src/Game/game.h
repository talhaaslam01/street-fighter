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
    int getFPS() const { return GetFPS(); }

    // Constants
    static const char *WINDOW_TITLE;
    static const int GAME_WIDTH = 384;
    static const int GAME_HEIGHT = 224;
    static const int GAME_SCALE_FACTOR = 3;
    static const int TARGET_FPS = 60;

private:
    void updateScalingRects();

    // Core loop functions
    void processInput();
    void update(float dt);
    void fixedUpdate(float fixedDt);
    void render();
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
};
