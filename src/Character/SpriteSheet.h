#pragma once

#include "Common.h"
#include "nlohmann/json.hpp"
#include "raylib.h"
#include <unordered_map>
#include <vector>

struct Sprite
{
    Rectangle source; // single sprite frame coordinates on the sheet {x, y, w, h}
    /*
     * Pivot is a reference point for aligning animations,
     * usually where the legs of the character are or will be if standing straight
     */
    Vector2 pivot;
};

class SpriteSheetManager
{
public:
    SpriteSheetManager() = default;
    ~SpriteSheetManager();

    void loadSpriteSheet(const char *characterName);
    void loadSpriteSheetMetaData(const char *characterName);

    const Sprite &getSprite(Action actionId, int frameIndex) const
    {
        return m_sprites.at(actionId).at(frameIndex);
    };

    const Texture2D &getSpriteSheet() const { return m_spriteSheet; }

private:
    std::unordered_map<Action, std::vector<Sprite>> m_sprites;
    Texture2D m_spriteSheet;
};