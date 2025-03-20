#include "game.h"
#include <iostream>

const char *Game::WINDOW_TITLE = "Street Fighter 2";

Game::Game()
    : m_targetFPS(TARGET_FPS),
      m_isRunning(false),
      m_windowWidth(GAME_WIDTH * GAME_SCALE_FACTOR),
      m_windowHeight(GAME_HEIGHT * GAME_SCALE_FACTOR),
      m_windowTitle(WINDOW_TITLE),
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
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(m_windowWidth, m_windowHeight, m_windowTitle);
    SetWindowMinSize(GAME_WIDTH, GAME_HEIGHT);
    SetTargetFPS(m_targetFPS);

    // Initialize raylib audio device
    InitAudioDevice();

    // Initialize rlImGui
    rlImGuiSetup(true);

    // Create render texture at original resolution
    m_gameTexture = LoadRenderTexture(GAME_WIDTH, GAME_HEIGHT);
    // Disable texture filtering for pixel-perfect rendering
    SetTextureFilter(m_gameTexture.texture, TEXTURE_FILTER_POINT);
    updateScalingRects();
}

void Game::updateScalingRects()
{
    // Set up source rectangle (entire game texture)
    m_sourceRect = {0.0f, 0.0f, static_cast<float>(GAME_WIDTH), static_cast<float>(-GAME_HEIGHT)};

    // Calculate aspect ratio
    float targetAspect = static_cast<float>(GAME_WIDTH) / GAME_HEIGHT;
    float windowAspect = static_cast<float>(m_windowWidth) / m_windowHeight;

    // Calculate destination rectangle (scaled to fit window while preserving aspect ratio)
    if (windowAspect > targetAspect)
    {
        // Window is wider than game aspect ratio
        float height = static_cast<float>(m_windowHeight);
        float width = height * targetAspect;
        float x = (m_windowWidth - width) / 2.0f;

        m_destRect = {x, 0.0f, width, height};
    }
    else
    {
        // Window is taller than game aspect ratio
        float width = static_cast<float>(m_windowWidth);
        float height = width / targetAspect;
        float y = (m_windowHeight - height) / 2.0f;

        m_destRect = {0.0f, y, width, height};
    }
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
            updateScalingRects();
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
    // Unload the render texture
    UnloadRenderTexture(m_gameTexture);

    // Close rlImGui
    rlImGuiShutdown();

    // Close raylib
    CloseAudioDevice();
    CloseWindow();
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
    // First render the game content at original resolution to the texture
    BeginTextureMode(m_gameTexture);
    {
        ClearBackground(BLACK);

        // Draw all game content here
        // For example:
        DrawRectangle(0, 0, GAME_WIDTH, GAME_HEIGHT, GREEN); // A test rectangle at original scale
        DrawText("Original SF2 Scale", GAME_WIDTH / 2, GAME_HEIGHT / 2, 10, WHITE);
    }
    EndTextureMode();

    // Then render the upscaled content to the main window
    BeginDrawing();
    {
        ClearBackground(BLACK);

        // Draw the scaled game texture
        DrawTexturePro(m_gameTexture.texture, m_sourceRect, m_destRect, {0.0f, 0.0f}, 0.0f, WHITE);

        // Start ImGui Content
        rlImGuiBegin();

        // bool open = true;
        // ImGui::ShowDemoWindow(&open);

        ImGui::Begin("Window Info");
        {
            ImGui::Text("Game Size: %d x %d", GAME_WIDTH, GAME_HEIGHT);
            ImGui::Text("Window Size: %d x %d", m_windowWidth, m_windowHeight);
            ImGui::Text("Game aspect ratio: %.3f", static_cast<float>(GAME_WIDTH) / GAME_HEIGHT);
            ImGui::Text("Window aspect ratio: %.3f", static_cast<float>(m_windowWidth) / m_windowHeight);
            ImGui::Text("Time: %.3f", m_totalTime);
            ImGui::Text("FPS: %d", GetFPS());
            ImGui::Text("Delta Time: %.6f", m_deltaTime);
            ImGui::Text("Fixed Delta Time: %.6f", getFixedDeltaTime());
        }
        ImGui::End();

        // End ImGui Content
        rlImGuiEnd();
    }
    EndDrawing();
}
