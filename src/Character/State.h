#pragma once

#include "Character.h"
#include "raylib.h"

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

enum class StateID
{
    // Common states
    Standing = 0,
    StandToCrouch = 10,
    Crouching = 11,
    CrouchToStand = 12,
    Walking = 20,
};

// Struct for state definition
struct StateDef
{
    StateID id;
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

// Structure to hold a complete state
struct State
{
    StateDef def;
    std::vector<StateController *> controllers;

    ~State()
    {
        for (auto ctrl : controllers)
            delete ctrl;
    }
};

// Base class for conditions
class Condition
{
public:
    virtual bool evaluate(Character *character) = 0;
    virtual ~Condition() {}
};

// Base class for controllers
class StateController
{
protected:
    std::vector<Condition *> conditions;

public:
    virtual void execute(Character *character) = 0;
    void addCondition(Condition *condition) { conditions.push_back(condition); }
    bool checkConditions(Character *character)
    {
        for (auto cond : conditions)
        {
            if (!cond->evaluate(character))
                return false;
        }
        return true;
    }
    virtual ~StateController()
    {
        for (auto cond : conditions)
            delete cond;
    }
};
