#pragma once

#include "raylib.h"

#include <deque>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Enum for directions
enum class Direction
{
    U,  // Up
    D,  // Down
    B,  // Back
    F,  // Forward
    UB, // Up Back
    UF, // Up Forward
    DB, // Down Back
    DF, // Down Forward
    N   // Neutral
};

// Enum for buttons
enum class Button : uint16_t
{
    n = 0,      // No button
    x = 1 << 0, // Button X, lp
    y = 1 << 1, // Button Y, mp
    z = 1 << 2, // Button Z, hp
    a = 1 << 3, // Button A, lk
    b = 1 << 4, // Button B, mk
    c = 1 << 5, // Button C, hk
    s = 1 << 6, // Start button
    o = 1 << 7  // Select button
};

// Bitwise operators for Button
inline Button operator|(Button a, Button b)
{
    return static_cast<Button>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
}

inline Button operator&(Button a, Button b)
{
    return static_cast<Button>(static_cast<uint16_t>(a) & static_cast<uint16_t>(b));
}

inline Button &operator|=(Button &a, Button b)
{
    a = a | b;
    return a;
}

inline bool hasFlag(Button combo, Button single)
{
    return static_cast<uint16_t>(combo & single) != 0;
}

// Enum for modifiers flags
enum class InputModifier : uint8_t
{
    NONE = 0,                // Regular press
    HOLD = 1 << 0,           // / - hold the key
    RELEASE = 1 << 1,        // ~ - release the key
    DIR_4WAY = 1 << 2,       // $ - Direction detected as 4-way
    SIMULTANEOUS = 1 << 3,   // + - Buttons pressed simultaneously
    NO_INTERRUPTION = 1 << 4 // > - No interruption between release and press (strict)
};

// Allow combining flags with bitwise operators
inline InputModifier operator|(InputModifier a, InputModifier b)
{
    return static_cast<InputModifier>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline InputModifier operator&(InputModifier a, InputModifier b)
{
    return static_cast<InputModifier>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

inline InputModifier &operator|=(InputModifier &a, InputModifier b)
{
    a = a | b;
    return a;
}

inline bool hasFlag(InputModifier flags, InputModifier flag)
{
    return static_cast<uint8_t>(flags & flag) != 0;
}

// Structure to hold a single input symbol
struct InputSymbol
{
    Direction direction;
    Button button;
    InputModifier modifier;
    float requiredHoldTime; // Required hold time in Ms

    InputSymbol()
        : direction(Direction::N),
          button(Button::n),
          modifier(InputModifier::NONE),
          requiredHoldTime(0.0f) {}

    bool isDirectional() const { return button == Button::n; }
    bool isButton() const { return direction == Direction::N; }
};

// Structure to hold a command definition
struct CommandDefinition
{
    std::string name;
    std::vector<InputSymbol> sequence;
    float time;       // Time to complete the command in Ms
    float bufferTime; // How long the command is buffered after successful execution in Ms

    static const int DEFAULT_TIME = 15;       // Default time for a command in frames
    static const int DEFAULT_BUFFER_TIME = 1; // Default buffer time in frames

    CommandDefinition()
        : name(""),
          time(0),
          bufferTime(0) {}

    CommandDefinition(std::string name, int targetFps, int time, int bufferTime)
        : name(name),
          time(time / targetFps * 1000),            // 15 frames at target FPS
          bufferTime(bufferTime / targetFps * 1000) // 1 frame at target FPS
    {
        if (time == 0)
        {
            this->time = DEFAULT_TIME / targetFps * 1000; // Default time in Ms
        }
        if (bufferTime == 0)
        {
            this->bufferTime = DEFAULT_BUFFER_TIME / targetFps * 1000; // Default buffer time in Ms
        }
    }

    void addSymbol(const InputSymbol &symbol)
    {
        sequence.push_back(symbol);
    }
};

// Structure to track input state for a specific input (button or direction)
struct InputState
{
    bool isPressed = false;
    bool wasPressed = false;
    float pressedTime = 0;  // How long has it been pressed in Ms
    float releasedTime = 0; // How long since it was released in Ms
};

// Structure to hold a single input frame
struct InputFrame
{
    std::unordered_map<Direction, InputState> directionStates;
    std::unordered_map<Button, InputState> buttonStates;
    float timestamp; // Timestamp of the frame in Ms
};

// Class to store and manage the input history
class InputBuffer
{
public:
    InputBuffer(int capacity = 60);
    ~InputBuffer() = default;

    void addFrame(const std::unordered_map<Direction, InputState> &directionStates,
                  const std::unordered_map<Button, InputState> &buttonStates);

    // Get current buffer
    const std::deque<InputFrame> &getBuffer() const
    {
        return m_buffer;
    }

    // Clear buffer
    void clear()
    {
        m_buffer.clear();
    }

private:
    const int m_maxCapacity;         // Maximum number of input symbols to store
    std::deque<InputFrame> m_buffer; // Buffer to store input frames
};

// Class to manage input for a character
class Input
{
public:
    Input(int m_targetFPS);
    ~Input() = default;

    void update(float dt);

    void registerCommand(const CommandDefinition &command);
    bool isCommandTriggered(const std::string &commandName) const;
    bool isButtonPressed(Button button) const;
    bool isButtonJustPressed(Button button) const;
    bool isButtonJustReleased(Button button) const;
    Direction getCurrentDirection() const { return m_currentDirection; }

private:
    void updateInputStates(float dt);
    void updateButtonState(Button button, bool isPressed, float dtInMs);
    void checkCommands();
    bool detectCommand(const CommandDefinition &command);
    bool matchSequence(const std::vector<InputSymbol> &sequence, const std::deque<InputFrame> &buffer, int framesWindow);

    InputBuffer m_inputBuffer;
    std::vector<CommandDefinition> m_commands;
    std::unordered_map<std::string, float> activeCommands; // Active commands and buffer time left in Ms

    std::unordered_map<Direction, InputState> m_directionStates;
    std::unordered_map<Button, InputState> m_buttonStates;
    Direction m_currentDirection;

    const int m_ticksInMs; // Time required for 1 tick/frame in ms
};
