#include "Animation.h"
#include "nlohmann/json.hpp"
#include <fstream>

using json = nlohmann::json;

void AnimationsManager::loadAnimationMetaData(const char *characterName)
{
    const auto filePath = "resources/characters/" + std::string(characterName) + "/anims.json";
    std::ifstream f(filePath);
    json meta = json::parse(f, nullptr, true, true);

    for (const auto &[action, animations] : meta.items())
    {
        const Action actionId = static_cast<Action>(std::stoi(action));

        AnimationGroup animationGroup;

        if (animations.contains("loopStartFrameIndex"))
        {
            animationGroup.loopStartFrameIndex = animations["loopStartFrameIndex"].get<int>();
        }

        for (const auto &clsn2Json : animations["defaultClsn2"])
        {
            Rectangle clsn2;
            clsn2.x = clsn2Json[0].get<float>();
            clsn2.y = clsn2Json[1].get<float>();
            clsn2.width = clsn2Json[2].get<float>();
            clsn2.height = clsn2Json[3].get<float>();
            animationGroup.defaultClsn2.emplace_back(clsn2);
        }

        for (const auto &frameJson : animations["frames"])
        {
            AnimationFrame frame;
            frame.frames = frameJson["frames"].get<int>();
            frame.offset.x = frameJson["offset"][0].get<float>();
            frame.offset.y = frameJson["offset"][1].get<float>();

            for (const auto &clsn1Json : frameJson["clsn1"])
            {
                Rectangle clsn1;
                clsn1.x = clsn1Json[0].get<float>();
                clsn1.y = clsn1Json[1].get<float>();
                clsn1.width = clsn1Json[2].get<float>();
                clsn1.height = clsn1Json[3].get<float>();
                frame.clsn1.emplace_back(clsn1);
            }

            for (const auto &clsn2Json : frameJson["clsn2"])
            {
                Rectangle clsn2;
                clsn2.x = clsn2Json[0].get<float>();
                clsn2.y = clsn2Json[1].get<float>();
                clsn2.width = clsn2Json[2].get<float>();
                clsn2.height = clsn2Json[3].get<float>();
                frame.clsn2.emplace_back(clsn2);
            }

            animationGroup.frames.emplace_back(frame);
        }

        m_animations[actionId] = animationGroup;
    }
}
