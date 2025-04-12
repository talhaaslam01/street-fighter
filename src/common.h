#pragma once

// Game constants
const int GAME_WIDTH = 384;
const int GAME_HEIGHT = 224;
const int GAME_SCALE_FACTOR = 3; // Game size scaling factor
const int FPS = 60;
constexpr float FRAME_TIME = 1.0f / FPS;              // Single frame time in seconds or fixed delta time
constexpr float FRAME_TIME_MS = FRAME_TIME * 1000.0f; // Single frame time in Ms or fixed delta time in Ms
