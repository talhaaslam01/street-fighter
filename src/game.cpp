#include "common.h"
#include "Game.h"

#include "imgui.h"
#include "rlImGui.h"

Game::Game()
    : m_isRunning(false),
      m_windowWidth(GAME_WIDTH * GAME_SCALE_FACTOR),
      m_windowHeight(GAME_HEIGHT * GAME_SCALE_FACTOR),
      m_stage("Suzaku Castle"),
      m_camera(GAME_WIDTH, GAME_HEIGHT, m_stage.getBounds()),
      m_player1(GAME_WIDTH, GAME_HEIGHT, true, m_stage, m_camera.getCamera()),
      m_player2(GAME_WIDTH, GAME_HEIGHT, false, m_stage, m_camera.getCamera()),
      m_isPaused(false),
      m_stepFrame(false)
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
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_MAXIMIZED);
    InitWindow(m_windowWidth, m_windowHeight, "Street Fighter 2");
    SetWindowMinSize(GAME_WIDTH, GAME_HEIGHT);
    SetTargetFPS(FPS);

    // Initialize raylib audio device
    InitAudioDevice();

    // Initialize rlImGui
    rlImGuiSetup(true);

    // Create render texture at original resolution
    // And, disable texture filtering for pixel-perfect rendering
    SetTextureFilter(m_gameTexture.texture, TEXTURE_FILTER_POINT);
    m_gameTexture = LoadRenderTexture(GAME_WIDTH, GAME_HEIGHT);
    updateScalingRects();

    // initialize entities
    m_stage.load();
    m_player1.initialize();
    m_player2.initialize();
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

    // Time tracking
    float deltaTime = 0.0f;
    double totalTime = 0.0;
    double accumulator = 0.0;

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
        deltaTime = GetFrameTime();
        if (deltaTime > 0.25)
        {
            deltaTime = 0.25;
        }

        // process input
        processInput();

        if (!m_isPaused || m_stepFrame) // Allow updates if not paused or stepping
        {
            // Update elapsed time
            totalTime += deltaTime;

            // Update game logic
            update(deltaTime);

            // Accumulator for fixed timestep
            accumulator += deltaTime;

            // Fixed update for physics
            while (accumulator >= FRAME_TIME)
            {
                fixedUpdate(FRAME_TIME);
                accumulator -= FRAME_TIME;
            }
        }

        // Render frame
        const float alpha = accumulator / FRAME_TIME;
        render(alpha);

        if (m_stepFrame)
        {
            m_stepFrame = false;
        }
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
    if (IsKeyPressed(KEY_F10))
    {
        m_isPaused = !m_isPaused;
    }

    if (IsKeyPressed(KEY_F11) && m_isPaused)
    {
        m_stepFrame = true;
    }
}

void Game::update(const float dt)
{
    // current scene update
    m_stage.update(dt);
    m_camera.update(m_player1.getPosition(), m_player2.getPosition(), dt);
    m_player1.update(dt);
}

void Game::fixedUpdate(const float fixedDt)
{
    // current scene fixed update
    m_player1.fixedUpdate(fixedDt);
}

void Game::render(const float alpha)
{
    // First render the game content at original resolution to the texture
    // TODO: Add rendering interpolation for smooth transitions
    BeginTextureMode(m_gameTexture);
    {
        ClearBackground(BLACK);

        // Camera mode rendering goes here
        BeginMode2D(m_camera.getCamera());
        {
            // Draw all game content here
            m_stage.render(m_camera.getCamera());
            m_player1.render();
            m_player2.render();
            m_camera.render();
        }
        EndMode2D();

        // Normal mode rendering goes here
    }
    EndTextureMode();

    // Then render the upscaled content to the main window
    BeginDrawing();
    {
        ClearBackground(BLACK);

        // Draw the scaled game texture
        DrawTexturePro(m_gameTexture.texture, m_sourceRect, m_destRect, {0.0f, 0.0f}, 0.0f, WHITE);

        imGuiDebugRender();
    }
    EndDrawing();
}

void Game::imGuiDebugRender()
{
    // Start ImGui Content
    rlImGuiBegin();

    bool open = true;
    ImGui::ShowDemoWindow(&open);

    ImGui::Begin("Window Info");
    {
        ImGui::Text("Game Size: %d x %d", GAME_WIDTH, GAME_HEIGHT);
        ImGui::Text("Window Size: %d x %d", m_windowWidth, m_windowHeight);
        ImGui::Text("Game aspect ratio: %.3f", static_cast<float>(GAME_WIDTH) / GAME_HEIGHT);
        ImGui::Text("Window aspect ratio: %.3f", static_cast<float>(m_windowWidth) / m_windowHeight);
        ImGui::Text("Time: %.3f", GetTime());
        ImGui::Text("FPS: %d", GetFPS());
        ImGui::Text("Delta Time: %.6f", GetFrameTime());
        ImGui::Text("Fixed Delta Time: %.6f", FRAME_TIME);
        ImGui::Text("Paused: %s", m_isPaused ? "true" : "false");
    }
    ImGui::End();

    // add more debug info here
    m_stage.imguiDebugRender(m_camera);
    m_camera.imguiDebugRender();
    m_player1.imGuiDebugRender();
    m_player2.imGuiDebugRender();

    // End ImGui Content
    rlImGuiEnd();
}
