#pragma once

#include "../Camera/GameCamera.h"

#include "raylib.h"
#include <string>
#include <vector>

struct BackgroundLayer
{
    Texture2D texture;
    Vector2 position;
    float parallaxFactor;
};

class Stage
{
public:
    Stage(const std::string &name);
    ~Stage();

    void load();
    void unload();
    void update(float dt);
    void render(const Camera2D &camera);

    // Get stage boundaries for camera and characters
    const Vector4 &getBounds() const { return m_bounds; }
    float getFloorY() const { return m_floorY; }
    float getStageCenterX() const { return (m_bounds.z - m_bounds.x) / 2.0f; }
    float getStageCenterY() const { return (m_bounds.w - m_bounds.y) / 2.0f; }

    // imgui debug
    void imguiDebugRender(GameCamera &camera);

private:
    std::string m_name;
    std::vector<BackgroundLayer> m_backgroundLayers;

    Vector4 m_bounds; // stage boundary (left, top, right, bottom)
    float m_floorY;

    // add animations vars here
};