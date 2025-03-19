#pragma once

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
    static const char *SCREEN_TITLE;
    static const int SCREEN_WIDTH = 800;
    static const int SCREEN_HEIGHT = 600;
    static const int TARGET_FPS = 60;

private:
    // Core loop functions
    void processInput();
    void update(float dt);
    void fixedUpdate(float fixedDt);
    void render();

    // core properties
    int m_windowWidth;
    int m_windowHeight;
    const char *m_windowTitle;
    int m_targetFPS;
    bool m_isRunning;

    // Time tracking
    float m_deltaTime;
    double m_totalTime;
    double m_accumulator;
};
