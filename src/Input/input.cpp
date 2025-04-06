#include "input.h"

#include <algorithm>

InputBuffer::InputBuffer(int capacity)
    : m_maxCapacity(capacity)
{
}

void InputBuffer::addFrame(const std::unordered_map<Direction, InputState> &directionStates,
                           const std::unordered_map<Button, InputState> &buttonStates)
{
    // Create a new frame entry
    InputFrame frame;
    frame.directionStates = directionStates;
    frame.buttonStates = buttonStates;
    frame.timestamp = static_cast<float>(GetTime() * 1000); // Convert to Ms

    // Add to buffer at the front (most recent frame)
    m_buffer.push_front(frame);

    // Remove frames which are older than 1 second
    while (!m_buffer.empty() && (frame.timestamp - m_buffer.back().timestamp) >= 1000.0f)
    {
        m_buffer.pop_back(); // Remove the oldest frame from the back
    }

    // Trim the buffer if it exceeds the maximum capacity
    if (static_cast<int>(m_buffer.size()) > m_maxCapacity)
    {
        m_buffer.pop_back(); // Remove the oldest frame from the back
    }
}

Input::Input(int m_targetFPS)
    : m_ticksInMs(m_targetFPS)
{
    m_directionStates[Direction::N] = InputState();
    m_directionStates[Direction::U] = InputState();
    m_directionStates[Direction::D] = InputState();
    m_directionStates[Direction::B] = InputState();
    m_directionStates[Direction::F] = InputState();
    m_directionStates[Direction::UB] = InputState();
    m_directionStates[Direction::UF] = InputState();
    m_directionStates[Direction::DB] = InputState();
    m_directionStates[Direction::DF] = InputState();

    m_buttonStates[Button::x] = InputState();
    m_buttonStates[Button::y] = InputState();
    m_buttonStates[Button::z] = InputState();
    m_buttonStates[Button::a] = InputState();
    m_buttonStates[Button::b] = InputState();
    m_buttonStates[Button::c] = InputState();
    m_buttonStates[Button::s] = InputState();
    m_buttonStates[Button::o] = InputState();
    m_buttonStates[Button::n] = InputState();

    m_currentDirection = Direction::N;
}

// Update input states and buffer them
void Input::update(float dt)
{
    updateInputStates(dt);

    // Add current frame to buffer
    m_inputBuffer.addFrame(m_directionStates, m_buttonStates);

    // Update command detection
    checkCommands();
}

// Register a command to be detected
void Input::registerCommand(const CommandDefinition &command)
{
    m_commands.push_back(command);
}

// Check if a command has been detected this frame
bool Input::isCommandTriggered(const std::string &commandName) const
{
    auto it = activeCommands.find(commandName);
    return it != activeCommands.end() && it->second > 0;
}

// Check if a button is pressed
bool Input::isButtonPressed(Button button) const
{
    auto it = m_buttonStates.find(button);
    return it != m_buttonStates.end() && it->second.isPressed;
}

// Check if a button was just pressed this frame
bool Input::isButtonJustPressed(Button button) const
{
    auto it = m_buttonStates.find(button);
    return it != m_buttonStates.end() && it->second.isPressed && !it->second.wasPressed;
}

// Check if a button was just released this frame
bool Input::isButtonJustReleased(Button button) const
{
    auto it = m_buttonStates.find(button);
    return it != m_buttonStates.end() && !it->second.isPressed && it->second.wasPressed;
}

// Update input states based on current input
void Input::updateInputStates(float dt)
{
    // Save previous states
    for (auto &pair : m_directionStates)
    {
        pair.second.wasPressed = pair.second.isPressed;
    }

    for (auto &pair : m_buttonStates)
    {
        pair.second.wasPressed = pair.second.isPressed;
    }

    // TODO: use config file to map keys to buttons and directions and handle character facing
    // Read current keyboard state
    const bool up = IsKeyDown(KEY_UP);
    const bool down = IsKeyDown(KEY_DOWN);
    const bool back = IsKeyDown(KEY_LEFT);
    const bool forward = IsKeyDown(KEY_RIGHT);

    // Determine current direction
    Direction newDirection = Direction::N;

    if (up && back)
        newDirection = Direction::UB;
    else if (up && forward)
        newDirection = Direction::UF;
    else if (down && back)
        newDirection = Direction::DB;
    else if (down && forward)
        newDirection = Direction::DF;
    else if (up)
        newDirection = Direction::U;
    else if (down)
        newDirection = Direction::D;
    else if (back)
        newDirection = Direction::B;
    else if (forward)
        newDirection = Direction::F;

    // Update direction states
    for (auto &pair : m_directionStates)
    {
        // Reset all directions to not pressed
        pair.second.isPressed = false;

        // If this direction was pressed and now isn't, reset pressed time and increment released time
        if (pair.second.wasPressed && !pair.second.isPressed)
        {
            pair.second.pressedTime = 0;
            pair.second.releasedTime += dt * 1000; // Convert to Ms
        }
    }

    // Set the current direction to pressed
    m_directionStates[newDirection].isPressed = true;

    // Update timing for current direction
    m_directionStates[newDirection].releasedTime = 0;

    const float dtInMs = dt * 1000; // Convert to Ms
    if (m_directionStates[newDirection].wasPressed)
    {
        m_directionStates[newDirection].pressedTime += dtInMs;
    }
    else
    {
        m_directionStates[newDirection].pressedTime = dtInMs;
    }

    m_currentDirection = newDirection;

    // TODO: will ned to change this to use isKeyPressed
    // Update button states
    updateButtonState(Button::x, IsKeyDown(KEY_Z), dtInMs); // Z key for X button
    updateButtonState(Button::y, IsKeyDown(KEY_X), dtInMs); // X key for Y button
    updateButtonState(Button::z, IsKeyDown(KEY_C), dtInMs); // C key for Z button
    updateButtonState(Button::a, IsKeyDown(KEY_A), dtInMs); // A key for A button
    updateButtonState(Button::b, IsKeyDown(KEY_S), dtInMs); // S key for B button
    updateButtonState(Button::c, IsKeyDown(KEY_D), dtInMs); // D key for C button
    // updateButtonState(Button::s, IsKeyDown(KEY_ENTER), dtInMs); // Enter for Start
    // updateButtonState(Button::o, IsKeyDown(KEY_SPACE), dtInMs); // Space for Select

    // Decrement buffer times for active commands
    for (auto it = activeCommands.begin(); it != activeCommands.end();)
    {
        it->second -= dt * 1000; // Decrement by the time since last frame in Ms
        if (it->second <= 0)
        {
            it = activeCommands.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

// Helper to update a button state
void Input::updateButtonState(Button button, bool isPressed, float dtInMs)
{
    InputState &state = m_buttonStates[button];
    state.isPressed = isPressed;

    if (state.isPressed)
    {
        if (state.wasPressed)
        {
            state.pressedTime += dtInMs;
        }
        else
        {
            state.pressedTime = dtInMs;
        }
        state.releasedTime = 0;
    }
    else
    {
        if (state.wasPressed)
        {
            state.pressedTime = 0;
            state.releasedTime = dtInMs;
        }
        else
        {
            state.releasedTime += dtInMs;
        }
    }
}

// Check for command sequences
void Input::checkCommands()
{
    // Go through each registered command
    for (const auto &command : m_commands)
    {
        bool detected = detectCommand(command);

        // If command detected, add to active commands list
        if (detected)
        {
            activeCommands[command.name] = command.bufferTime;
        }
    }
}

// Check if a specific command has been input
bool Input::detectCommand(const CommandDefinition &command)
{
    float timeWindow = command.time;
    float currentTime = static_cast<float>(GetTime() * 1000); // Get current time in Ms

    // Get the buffer frames within the command time window
    const auto &buffer = m_inputBuffer.getBuffer();

    auto it = std::find_if(buffer.begin(), buffer.end(),
                           [currentTime, timeWindow](const InputFrame &frame)
                           {
                               return (currentTime - frame.timestamp) > timeWindow;
                           });

    // The number of frames within the time window is the distance from the start to the found iterator
    int framesNeeded = std::distance(buffer.begin(), it);

    // Skip if not enough frames in buffer
    if (framesNeeded < command.sequence.size())
    {
        return false;
    }

    // Start from the most recent frame and check if the command sequence is met
    return matchSequence(command.sequence, buffer, framesNeeded);
}

// Match a command sequence against the input buffer
bool Input::matchSequence(const std::vector<InputSymbol> &sequence, const std::deque<InputFrame> &buffer, int framesWindow)
{
    // TODO: Implement actual sequence matching
    // This is a simplified version; real implementation needs to handle all special cases:
    // - Held inputs (/)
    // - Releases (~)
    // - Direction-only ($)
    // - Simultaneous presses (+)
    // - No interruption (>)

    return false; // Placeholder
}
