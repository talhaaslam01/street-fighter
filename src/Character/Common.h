#pragma once

#include "raylib.h"

enum class Action
{
    // purely for animation
    STANDING = 0,
    WALKING_FORWARD = 20,
    WALKING_BACKWARD = 21
};

enum class CharacterState
{
    STANDING = 0,
    WALKING_FORWARD = 20,
    WALKING_BACKWARD = 21
};

// Enums for state types
enum class StateType
{
    Standing,  // S
    Crouching, // C
    Airborne   // A
};
enum class MoveType
{
    Idle,   // I
    Attack, // A
    Hit     // H
};
enum class PhysicsType
{
    Standing,  // S
    Crouching, // C
    Airborne,  // A
    None       // N
};

// Struct for state definition
struct StateDef
{
    int id;
    StateType type;
    MoveType moveType;
    PhysicsType physics;
    int anim;
    Vector2 velset;
    bool ctrl;
    int powerAdd;
    int juggle;
    bool facep2;
    bool hitdefpersist;
    bool movehitpersist;
    bool hitcountpersist;
    int sprpriority;
};