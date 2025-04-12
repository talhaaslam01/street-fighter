#include "input.h"
#include "parser.h"

#include <algorithm>

// Maximum time for a button press/release in Ms (10 seconds)
const float Input::MAX_TIME = 10000.0f;
const int Input::SIMULTANEOUS_WINDOW_FRAMES = 5;

InputBuffer::InputBuffer(int capacity)
    : m_maxCapacity(capacity)
{
}

void InputBuffer::addFrame(const std::unordered_map<Direction, InputState> &directionStates,
                           const std::unordered_map<Button, InputState> &buttonStates,
                           const double gameTime)
{
    // Create a new frame entry
    InputFrame frame;
    frame.directionStates = directionStates;
    frame.buttonStates = buttonStates;
    frame.timestampMs = static_cast<float>(gameTime * 1000);

    // Add to buffer at the front (most recent frame)
    m_buffer.push_front(frame);

    // Remove frames which are older than 1 second
    while (!m_buffer.empty() && (frame.timestampMs - m_buffer.back().timestampMs) >= 1000.0f)
    {
        m_buffer.pop_back(); // Remove the oldest frame from the back
    }

    // Trim the buffer if it exceeds the maximum capacity
    if (static_cast<int>(m_buffer.size()) > m_maxCapacity)
    {
        m_buffer.pop_back(); // Remove the oldest frame from the back
    }
}

Input::Input(const char *characterName)
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

    CommandParser::parseCommandFile(std::string(characterName), *this);
}

void Input::update(const float dt, const double totalTime)
{
    updateInputStates(dt);

    // Add current frame to buffer
    m_inputBuffer.addFrame(m_directionStates, m_buttonStates, totalTime);

    // Update command detection
    checkCommands(totalTime);
}

void Input::registerCommand(const CommandDefinition &command)
{
    m_commands.push_back(command);
}

bool Input::isCommandTriggered(const std::string &commandName) const
{
    auto it = m_activeCommandsTimeLeftMs.find(commandName);
    return it != m_activeCommandsTimeLeftMs.end() && it->second > 0;
}

bool Input::isButtonPressed(const Button button) const
{
    auto it = m_buttonStates.find(button);
    return it != m_buttonStates.end() && it->second.isPressed;
}

bool Input::isButtonJustPressed(const Button button) const
{
    auto it = m_buttonStates.find(button);
    return it != m_buttonStates.end() && it->second.isPressed && !it->second.wasPressed;
}

bool Input::isButtonJustReleased(const Button button) const
{
    auto it = m_buttonStates.find(button);
    return it != m_buttonStates.end() && !it->second.isPressed && it->second.wasPressed;
}

void Input::updateInputStates(const float dt)
{
    const float dtMs = dt * 1000;

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
        bool isCurrentDirection = (pair.first == newDirection);
        pair.second.wasPressed = pair.second.isPressed; // Save previous state
        pair.second.isPressed = isCurrentDirection;     // Set current direction state

        // update timings
        if (isCurrentDirection)
        {
            pair.second.releasedTimeMs = 0.0f;
            pair.second.pressedTimeMs = std::min(pair.second.pressedTimeMs + dtMs, MAX_TIME);
        }
        else
        {
            pair.second.pressedTimeMs = 0.0f;
            pair.second.releasedTimeMs = std::min(pair.second.releasedTimeMs + dtMs, MAX_TIME);
        }
    }

    m_currentDirection = newDirection;

    // Update button states
    updateButtonState(Button::x, IsKeyPressed(KEY_A), dtMs); // Z key for X button
    updateButtonState(Button::y, IsKeyPressed(KEY_S), dtMs); // X key for Y button
    updateButtonState(Button::z, IsKeyPressed(KEY_D), dtMs); // C key for Z button
    updateButtonState(Button::a, IsKeyPressed(KEY_Z), dtMs); // A key for A button
    updateButtonState(Button::b, IsKeyPressed(KEY_X), dtMs); // S key for B button
    updateButtonState(Button::c, IsKeyPressed(KEY_C), dtMs); // D key for C button

    // Decrement buffer times for active commands
    for (auto it = m_activeCommandsTimeLeftMs.begin(); it != m_activeCommandsTimeLeftMs.end();)
    {
        it->second -= dtMs; // Decrement by the time since last frame
        if (it->second <= 0)
        {
            it = m_activeCommandsTimeLeftMs.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void Input::updateButtonState(const Button button, const bool isPressed, const float dtMs)
{
    InputState &state = m_buttonStates[button];
    state.wasPressed = state.isPressed; // Save previous state
    state.isPressed = isPressed;

    if (state.isPressed)
    {
        state.pressedTimeMs = std::min(state.pressedTimeMs + dtMs, MAX_TIME);
        state.releasedTimeMs = 0.0f;
    }
    else
    {
        state.pressedTimeMs = 0.0f;
        state.releasedTimeMs = std::min(state.releasedTimeMs + dtMs, MAX_TIME);
    }
}

void Input::checkCommands(const double totalTime)
{
    // Go through each registered command
    for (const auto &command : m_commands)
    {
        bool detected = detectCommand(command, totalTime);

        // If command detected, add to active commands list
        if (detected)
        {
            m_activeCommandsTimeLeftMs[command.name] = command.bufferTimeMs;
        }
    }
}

bool Input::detectCommand(const CommandDefinition &command, const double totalTime)
{
    float timeWindowMs = command.timeMs;
    float currentTimeMs = static_cast<float>(totalTime * 1000);

    // Get the buffer frames within the command time window
    const auto &buffer = m_inputBuffer.getBuffer();

    // Find the first frame that's outside the time window (going from most recent to oldest)
    auto timeWindowEnd = std::find_if(buffer.begin(), buffer.end(),
                                      [currentTimeMs, timeWindowMs](const InputFrame &frame)
                                      {
                                          return (currentTimeMs - frame.timestampMs) > timeWindowMs;
                                      });

    // Calculate number of frames within the time window
    int framesInWindow = std::distance(buffer.begin(), timeWindowEnd);

    // Skip if not enough frames or if sequence is too long for window
    if (framesInWindow < static_cast<int>(command.sequence.size()))
    {
        return false;
    }

    // Call matchSequence with only the relevant part of the buffer
    return matchSequence(command.sequence, buffer, framesInWindow);
}

bool Input::matchSequence(const std::vector<InputSymbol> &sequence, const std::deque<InputFrame> &buffer, const int framesWindow)
{
    if (sequence.empty() || buffer.empty())
    {
        return false;
    }

    // We need to scan through the buffer to find matching patterns
    size_t currentSeqIndex = 0;                // Start with the first input in the sequence
    std::vector<InputSymbol> activeHeldInputs; // Track inputs that need to be held

    size_t maxBufferIndex = std::min(buffer.size(), static_cast<size_t>(framesWindow));
    size_t startBufferIndex = 0;

    // Try to find the first input in the sequence
    while (startBufferIndex < maxBufferIndex)
    {
        if (matchSymbol(sequence[0], buffer[startBufferIndex]))
        {
            break;
        }
        startBufferIndex++;
    }

    // If we couldn't find the first input,
    // or if the sequence is longer than the buffer, fail immediately
    if (startBufferIndex >= maxBufferIndex ||
        (maxBufferIndex - startBufferIndex) < sequence.size())
    {
        return false;
    }

    // Found first input, now try to match the rest of the sequence
    size_t bufferIndex = startBufferIndex;
    currentSeqIndex = 0;

    while (currentSeqIndex < sequence.size() && bufferIndex < maxBufferIndex)
    {
        const InputSymbol &symbol = sequence[currentSeqIndex];
        const InputFrame &frame = buffer[bufferIndex];

        // First check all active held inputs - they must still be pressed
        bool allHeldInputsActive = true;
        for (const auto &heldSymbol : activeHeldInputs)
        {
            if (!isInputActive(heldSymbol, frame))
            {
                allHeldInputsActive = false;
                break;
            }
        }

        // If any held input is no longer active, the sequence fails
        if (!allHeldInputsActive)
        {
            return false;
        }

        // Check if the current symbol matches
        bool matched = matchSymbol(symbol, frame);

        // Handle no interruption modifier
        if (matched && hasFlag(symbol.modifier, InputModifier::NO_INTERRUPTION) &&
            currentSeqIndex > 0 && bufferIndex > 0)
        {
            // Check for interruptions between current and last matched frame
            // TODO: might need to fix this
            if (hasInterruption(buffer[bufferIndex - 1], frame, sequence[currentSeqIndex - 1], symbol))
            {
                matched = false;
            }
        }

        if (matched)
        {
            // If this is a held input, add it to our tracking list
            if (hasFlag(symbol.modifier, InputModifier::HOLD))
            {
                activeHeldInputs.push_back(symbol);
            }

            // Move to next symbol in sequence
            currentSeqIndex++;

            // If we've matched the entire sequence, success!
            if (currentSeqIndex >= sequence.size())
            {
                return true;
            }
        }

        // Always move to the next buffer frame
        bufferIndex++;
    }

    // If we get here, we didn't match the complete sequence
    return false;
}

bool Input::matchSymbol(const InputSymbol &symbol, const InputFrame &frame)
{
    bool matched = false;

    // Handle directional input
    if (symbol.isDirectional())
    {
        Direction dirToCheck = symbol.direction;
        auto dirIt = frame.directionStates.find(dirToCheck);

        // Handle release (~), direction is released
        if (hasFlag(symbol.modifier, InputModifier::RELEASE))
        {
            // handle 4-way release
            if (hasFlag(symbol.modifier, InputModifier::DIR_4WAY))
            {
                matched = match4WayRelease(symbol, frame);
            }
            // normal release
            else if (dirIt != frame.directionStates.end())
            {
                matched = !dirIt->second.isPressed && dirIt->second.wasPressed &&
                          (symbol.requiredHoldTimeMs <= 0.0f || dirIt->second.releasedTimeMs >= symbol.requiredHoldTimeMs);
            }
        }
        // handle 4-way press ($)
        else if (hasFlag(symbol.modifier, InputModifier::DIR_4WAY))
        {
            // Check if any of the corresponding directions are active
            if (dirToCheck == Direction::U)
            {
                matched = frame.directionStates.at(Direction::U).isPressed ||
                          frame.directionStates.at(Direction::UB).isPressed ||
                          frame.directionStates.at(Direction::UF).isPressed;
            }
            else if (dirToCheck == Direction::D)
            {
                matched = frame.directionStates.at(Direction::D).isPressed ||
                          frame.directionStates.at(Direction::DB).isPressed ||
                          frame.directionStates.at(Direction::DF).isPressed;
            }
            else if (dirToCheck == Direction::B)
            {
                matched = frame.directionStates.at(Direction::B).isPressed ||
                          frame.directionStates.at(Direction::UB).isPressed ||
                          frame.directionStates.at(Direction::DB).isPressed;
            }
            else if (dirToCheck == Direction::F)
            {
                matched = frame.directionStates.at(Direction::F).isPressed ||
                          frame.directionStates.at(Direction::UF).isPressed ||
                          frame.directionStates.at(Direction::DF).isPressed;
            }
        }
        // handle normal press
        else if (dirIt != frame.directionStates.end())
        {
            matched = dirIt->second.isPressed;
        }
    }
    // Handle button input
    else if (symbol.button != Button::n)
    {
        // Handle simultaneous press (+)
        if (hasFlag(symbol.modifier, InputModifier::SIMULTANEOUS))
        {
            // Check simultaneous button presses with relaxation window

            // First check if all required buttons are currently pressed
            bool allButtonsPressed = true;
            std::vector<Button> requiredButtons;

            // Collect all required buttons in the combination
            for (uint16_t btn = 1; btn <= static_cast<uint16_t>(Button::o); btn <<= 1)
            {
                Button btnFlag = static_cast<Button>(btn);
                if (hasFlag(symbol.button, btnFlag))
                {
                    auto btnIt = frame.buttonStates.find(btnFlag);
                    if (btnIt == frame.buttonStates.end() || !btnIt->second.isPressed)
                    {
                        allButtonsPressed = false;
                        break;
                    }
                    requiredButtons.push_back(btnFlag);
                }
            }

            // If not all buttons are pressed in this frame, no match
            if (!allButtonsPressed)
            {
                matched = false;
            }
            else
            {
                // Check if all buttons were pressed within the relaxation window
                float earliestPressTime = std::numeric_limits<float>::max();
                float latestPressTime = 0;

                for (Button btn : requiredButtons)
                {
                    float pressTime = frame.buttonStates.at(btn).pressedTimeMs;
                    earliestPressTime = std::min(earliestPressTime, pressTime);
                    latestPressTime = std::max(latestPressTime, pressTime);
                }

                // Convert frame relaxation to milliseconds using tick time
                float maxAllowedDifferenceMs = SIMULTANEOUS_WINDOW_FRAMES * FRAME_TIME_MS;

                // Compare time difference between earliest and latest button press
                matched = (latestPressTime - earliestPressTime) <= maxAllowedDifferenceMs;
            }

            return matched;
        }
        else
        {
            auto btnIt = frame.buttonStates.find(symbol.button);
            if (btnIt != frame.buttonStates.end())
            {
                // Handle release (~) or regular press
                if (hasFlag(symbol.modifier, InputModifier::RELEASE))
                {
                    return !btnIt->second.isPressed && btnIt->second.wasPressed &&
                           (symbol.requiredHoldTimeMs <= 0.0f || btnIt->second.releasedTimeMs >= symbol.requiredHoldTimeMs);
                }
                else
                {
                    // Both regular press and hold modifier check isPressed
                    return btnIt->second.isPressed;
                }
            }
        }
    }

    return matched;
}

bool Input::match4WayRelease(const InputSymbol &symbol, const InputFrame &frame)
{
    Direction dirToCheck = symbol.direction;
    bool matched = false;

    auto isDirReleasedOnTime = [](const Direction dir, const InputFrame &frame, const InputSymbol &symbol)
    {
        return !frame.directionStates.at(dir).isPressed && frame.directionStates.at(dir).wasPressed &&
               (symbol.requiredHoldTimeMs <= 0.0f ||
                frame.directionStates.at(dir).releasedTimeMs >= symbol.requiredHoldTimeMs);
    };

    if (dirToCheck == Direction::U)
    {
        const bool isUReleasedOnTime = isDirReleasedOnTime(Direction::U, frame, symbol);
        const bool isUBReleasedOnTime = isDirReleasedOnTime(Direction::UB, frame, symbol);
        const bool isUFReleasedOnTime = isDirReleasedOnTime(Direction::UF, frame, symbol);

        matched = isUReleasedOnTime || isUBReleasedOnTime || isUFReleasedOnTime;
    }
    else if (dirToCheck == Direction::D)
    {
        const bool isDReleasedOnTime = isDirReleasedOnTime(Direction::D, frame, symbol);
        const bool isDBReleasedOnTime = isDirReleasedOnTime(Direction::DB, frame, symbol);
        const bool isDFReleasedOnTime = isDirReleasedOnTime(Direction::DF, frame, symbol);

        matched = isDReleasedOnTime || isDBReleasedOnTime || isDFReleasedOnTime;
    }
    else if (dirToCheck == Direction::B)
    {
        const bool isBReleasedOnTime = isDirReleasedOnTime(Direction::B, frame, symbol);
        const bool isDBReleasedOnTime = isDirReleasedOnTime(Direction::DB, frame, symbol);
        const bool isUBReleasedOnTime = isDirReleasedOnTime(Direction::UB, frame, symbol);

        matched = isBReleasedOnTime || isDBReleasedOnTime || isUBReleasedOnTime;
    }
    else if (dirToCheck == Direction::F)
    {
        const bool isFReleasedOnTime = isDirReleasedOnTime(Direction::F, frame, symbol);
        const bool isDFReleasedOnTime = isDirReleasedOnTime(Direction::DF, frame, symbol);
        const bool isUFReleasedOnTime = isDirReleasedOnTime(Direction::UF, frame, symbol);

        matched = isFReleasedOnTime || isDFReleasedOnTime || isUFReleasedOnTime;
    }

    return matched;
}

bool Input::isInputActive(const InputSymbol &symbol, const InputFrame &frame)
{
    if (symbol.isDirectional())
    {

        // check if the 4-way direction is active
        if (hasFlag(symbol.modifier, InputModifier::DIR_4WAY))
        {
            Direction dirToCheck = symbol.direction;
            if (dirToCheck == Direction::U)
            {
                return frame.directionStates.at(Direction::U).isPressed ||
                       frame.directionStates.at(Direction::UB).isPressed ||
                       frame.directionStates.at(Direction::UF).isPressed;
            }
            else if (dirToCheck == Direction::D)
            {
                return frame.directionStates.at(Direction::D).isPressed ||
                       frame.directionStates.at(Direction::DB).isPressed ||
                       frame.directionStates.at(Direction::DF).isPressed;
            }
            else if (dirToCheck == Direction::B)
            {
                return frame.directionStates.at(Direction::B).isPressed ||
                       frame.directionStates.at(Direction::UB).isPressed ||
                       frame.directionStates.at(Direction::DB).isPressed;
            }
            else if (dirToCheck == Direction::F)
            {
                return frame.directionStates.at(Direction::F).isPressed ||
                       frame.directionStates.at(Direction::UF).isPressed ||
                       frame.directionStates.at(Direction::DF).isPressed;
            }
        }

        // check if the direction is active
        auto dirIt = frame.directionStates.find(symbol.direction);
        return dirIt != frame.directionStates.end() && dirIt->second.isPressed;
    }
    else if (symbol.isButton())
    {
        auto btnIt = frame.buttonStates.find(symbol.button);
        return btnIt != frame.buttonStates.end() && btnIt->second.isPressed;
    }
    return false;
}

// Helper to check for interruptions between frames
bool Input::hasInterruption(const InputFrame &prevFrame, const InputFrame &currFrame,
                            const InputSymbol &prevSymbol, const InputSymbol &currSymbol)
{
    // Check if any direction changed state other than the ones in the sequence
    for (const auto &dirPair : currFrame.directionStates)
    {
        Direction dir = dirPair.first;
        const InputState &currState = dirPair.second;
        const InputState &prevState = prevFrame.directionStates.at(dir);

        // If the state changed (pressed or released)
        if (currState.isPressed != prevState.isPressed)
        {
            // Check if this direction is part of our sequence
            bool isInSequence = (dir == prevSymbol.direction || dir == currSymbol.direction);

            // If not part of our sequence, it's an interruption
            if (!isInSequence)
            {
                return true;
            }
        }
    }

    // Check if any button changed state other than the ones in the sequence
    for (const auto &btnPair : currFrame.buttonStates)
    {
        Button btn = btnPair.first;
        const InputState &currState = btnPair.second;
        const InputState &prevState = prevFrame.buttonStates.at(btn);

        // If the state changed (pressed or released)
        if (currState.isPressed != prevState.isPressed)
        {
            // For simultaneous press, check if this button is part of the combination
            bool isInSequence = false;

            if (hasFlag(prevSymbol.modifier, InputModifier::SIMULTANEOUS) && prevSymbol.button != Button::n)
            {
                isInSequence = hasFlag(prevSymbol.button, btn);
            }
            else if (hasFlag(currSymbol.modifier, InputModifier::SIMULTANEOUS) && currSymbol.button != Button::n)
            {
                isInSequence = hasFlag(currSymbol.button, btn);
            }
            else
            {
                // For regular buttons, direct comparison
                isInSequence = (btn == prevSymbol.button || btn == currSymbol.button);
            }

            // If not part of our sequence, it's an interruption
            if (!isInSequence)
            {
                return true;
            }
        }
    }

    // No interruptions found
    return false;
}

#include "imgui.h"
void Input::imGuiDebugRender()
{
    std::string commands = "";

    for (const auto &command : m_activeCommandsTimeLeftMs)
    {
        commands += command.first + ", ";
    }

    ImGui::Text("Commands: %s", commands.c_str());
}
