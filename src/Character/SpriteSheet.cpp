#include "SpriteSheet.h"
#include "nlohmann/json.hpp"
#include <fstream>

using json = nlohmann::json;

SpriteSheetManager::~SpriteSheetManager()
{
    UnloadTexture(m_spriteSheet);
}

void SpriteSheetManager::loadSpriteSheet(const char *characterName)
{
    const auto filePath = "resources/characters/" + std::string(characterName) + "/sprites.png";
    Image img = LoadImage(filePath.c_str());
    Color bg = GetImageColor(img, 0, 0);

    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8); // Ensure proper transparency
    ImageColorReplace(&img, bg, BLANK);
    m_spriteSheet = LoadTextureFromImage(img);
    UnloadImage(img); // Unload image from RAM (texture is now in VRAM)
}

void SpriteSheetManager::loadSpriteSheetMetaData(const char *characterName)
{
    const auto filePath = "resources/characters/" + std::string(characterName) + "/sprites.json";
    std::ifstream f(filePath);
    json meta = json::parse(f, nullptr, true, true);

    for (const auto &[action, sprites] : meta.items())
    {
        const Action actionId = static_cast<Action>(std::stoi(action));

        for (const auto &spriteJson : sprites)
        {
            Sprite sprite;
            sprite.source.x = spriteJson["source"][0].get<float>();
            sprite.source.y = spriteJson["source"][1].get<float>();
            sprite.source.width = spriteJson["source"][2].get<float>();
            sprite.source.height = spriteJson["source"][3].get<float>();
            sprite.pivot.x = spriteJson["pivot"][0].get<float>();
            sprite.pivot.y = spriteJson["pivot"][1].get<float>();

            m_sprites[actionId].emplace_back(sprite);
        }
    }
}
