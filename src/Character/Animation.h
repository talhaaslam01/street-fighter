#pragma once

#include "Common.h"
#include "raylib.h"
#include <vector>
#include <unordered_map>

// Single animation frame
struct AnimationFrame
{
    // Action group;                 // sprite group number
    // int frameIndex;               // index of the sprite in the group
    Vector2 offset;               // offset from the character position
    int frames;                   // number of frames in the animation before moving to the next, -1 for infinite (no loop)
    std::vector<Rectangle> clsn1; // atack hitboxes
    std::vector<Rectangle> clsn2; // hurt hitboxes
    // bool flipH;                 // flip sprite horizontally
    // bool flipV;                 // flip sprite vertically
    // Vector2 scale;              // scale sprite
    // int rotation;               // rotate sprite
};

// One completion action animation
struct AnimationGroup
{
    // Use these collision by default if no hitboxes are specified in the animation frames
    std::vector<Rectangle> defaultClsn2;
    std::vector<AnimationFrame> frames;
    int loopStartFrameIndex{}; // repeats the animation from this frame
};

class AnimationsManager
{
public:
    AnimationsManager() = default;
    ~AnimationsManager() = default;

    void loadAnimationMetaData(const char *characterName);

    const AnimationGroup &getAnimationGroup(Action actionId) const
    {
        return m_animations.at(actionId);
    }

private:
    std::unordered_map<Action, AnimationGroup> m_animations;
};
