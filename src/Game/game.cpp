#include "game.h"
#include <iostream>

const char *Game::SCREEN_TITLE = "Street Fighter 2";

Game::Game()
    : m_windowWidth(SCREEN_WIDTH),
      m_windowHeight(SCREEN_HEIGHT),
      m_windowTitle(SCREEN_TITLE),
      m_targetFPS(TARGET_FPS),
      m_isRunning(false),
      m_deltaTime(0.0f),
      m_totalTime(0.0),
      m_accumulator(0.0)
{
}

Game::~Game()
{
    shutdown();
}

void Game::initialize()
{
    m_isRunning = true;

    // Initialize raylib window
    // SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(m_windowWidth, m_windowHeight, m_windowTitle);
    SetTargetFPS(m_targetFPS);
    rlImGuiSetup(true);

    std::cout << "Core Engine initialized" << std::endl;
}

void Game::run()
{
    if (!m_isRunning)
    {
        initialize();
    }

    // game loop
    while (!WindowShouldClose() && m_isRunning)
    {
        if (IsWindowResized())
        {
            m_windowWidth = GetScreenWidth();
            m_windowHeight = GetScreenHeight();
        }

        // Calculate delta time
        m_deltaTime = GetFrameTime();
        m_totalTime += m_deltaTime;

        // process input
        processInput();

        // Update game logic
        update(m_deltaTime);

        // Accumulator for fixed timestep
        m_accumulator += m_deltaTime;

        // Fixed update for physics
        while (m_accumulator >= getFixedDeltaTime())
        {
            fixedUpdate(getFixedDeltaTime());
            m_accumulator -= getFixedDeltaTime();
        }

        // Render frame
        render();
    }
}

void Game::shutdown()
{
    // Close rlImGui
    rlImGuiShutdown();

    // Close raylib
    CloseAudioDevice();
    CloseWindow();

    std::cout << "Core Engine shutdown" << std::endl;
}

void Game::processInput()
{
    // current scene process input
}

void Game::update(float dt)
{
    // current scene update
}

void Game::fixedUpdate(float fixedDt)
{
    // current scene fixed update
}

void Game::render()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // start ImGui Content
    rlImGuiBegin();

    bool open = true;
    ImGui::ShowDemoWindow(&open);

    // end ImGui Content
    rlImGuiEnd();

    // current scene render

    // Debug info - FPS counter
    DrawFPS(10, 10);

    EndDrawing();
}
