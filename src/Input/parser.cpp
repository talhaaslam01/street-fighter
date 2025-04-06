#include "parser.h"

using json = nlohmann::json;

CommandParser::CommandParser(int targetFPS)
    : m_targetFPS(targetFPS)
{
}

bool CommandParser::parseCommandFile(const std::string &characterName, Input &input)
{
    const auto filePath = "resources/characters/" + std::string(characterName) + "/cmds.jsonc";
    std::ifstream file(filePath);
    json cmdsJson = json::parse(file, nullptr, true, true);

    auto commands = cmdsJson["commands"];
    for (const auto &command : commands)
    {
        CommandDefinition cmd = parseCommandSection(command);
        input.registerCommand(cmd);
    }

    return true;
}

CommandDefinition CommandParser::parseCommandSection(const json &command)
{
    std::string name = command["name"].get<std::string>();
    std::string commandStr = command["command"].get<std::string>();
    int time = command.value("time", 0);
    int bufferTime = command.value("bufferTime", 0);

    CommandDefinition cmd(name, m_targetFPS, time, bufferTime);

    std::vector<InputSymbol> sequence = parseCommandSequence(commandStr);
    for (const auto &symbol : sequence)
    {
        cmd.addSymbol(symbol);
    }
    return cmd;
}

std::vector<InputSymbol> CommandParser::parseCommandSequence(const std::string &commandStr)
{
    std::vector<InputSymbol> sequence;
    std::vector<std::string> symbols = split(commandStr, ',');

    for (auto &symbolStr : symbols)
    {
        InputSymbol symbol = parseInputSymbol(trim(symbolStr));
        sequence.push_back(symbol);
    }

    // Handle special case: consecutive directional inputs
    // Example: F, F becomes F, >~F, >F
    std::vector<InputSymbol> expandedSequence;

    for (size_t i = 0; i < sequence.size(); i++)
    {
        const InputSymbol &current = sequence[i];
        const InputSymbol &prev = (i > 0) ? sequence[i - 1] : InputSymbol{};

        if (i > 0 && current.isDirectional() && prev.isDirectional() &&
            current.direction == prev.direction &&
            current.modifier == InputModifier::NONE &&
            prev.modifier == InputModifier::NONE)
        {
            // Insert a no-interrupt & release modifier of the previous direction
            InputSymbol strictReleaseSymbol;
            strictReleaseSymbol.direction = current.direction;
            strictReleaseSymbol.modifier = InputModifier::RELEASE | InputModifier::NO_INTERRUPTION;
            expandedSequence.push_back(strictReleaseSymbol);

            // Now add the current direction with no interruption modifier
            InputSymbol strictSymbol;
            strictSymbol.direction = current.direction;
            strictSymbol.modifier = InputModifier::NO_INTERRUPTION;
            expandedSequence.push_back(strictSymbol);
        }
        else
        {
            expandedSequence.push_back(current);
        }
    }

    return expandedSequence;
}

InputSymbol CommandParser::parseInputSymbol(const std::string &symbolStr)
{
    InputSymbol symbol;
    std::string str = symbolStr;

    // check for ">" modifier
    if (!str.empty() && str[0] == '>')
    {
        symbol.modifier |= InputModifier::NO_INTERRUPTION;
        str.erase(0, 1); // Remove the ">" character
    }

    // Check for release (~) or hold (/) modifier
    if (!str.empty() && str[0] == '~')
    {
        symbol.modifier |= InputModifier::RELEASE;
        str.erase(0, 1); // Remove the "~" character

        // optional frame number after ~
        size_t i = 0;
        while (i < str.size() && std::isdigit(str[i]))
            ++i;

        if (i > 0)
        {
            symbol.requiredHoldTime = std::stoi(str.substr(0, i)) / static_cast<float>(m_targetFPS);
            str.erase(0, i); // Remove the number
        }
    }
    else if (!str.empty() && str[0] == '/')
    {
        symbol.modifier |= InputModifier::HOLD;
        str.erase(0, 1);
    }

    // Check for 4-way direction ($)
    if (!str.empty() && str[0] == '$')
    {
        symbol.modifier |= InputModifier::DIR_4WAY;
        str.erase(0, 1); // Remove the "$" character
    }

    // Handle direction or button combo
    if (str.find('+') != std::string::npos)
    {
        symbol.modifier = InputModifier::SIMULTANEOUS;

        for (char ch : str)
        {
            if (ch != '+')
            {
                symbol.button |= parseButton(ch);
            }
        }
    }
    else
    {
        Button b = parseButton(str[0]);
        Direction d = parseDirection(str[0]);

        if (b != Button::n)
        {
            symbol.button = b;
        }
        else if (d != Direction::N)
        {
            symbol.direction = d;
        }
    }
}

Direction CommandParser::parseDirection(const char dirCh)
{
    if (dirCh == 'U')
        return Direction::U;
    if (dirCh == 'D')
        return Direction::D;
    if (dirCh == 'B')
        return Direction::B;
    if (dirCh == 'F')
        return Direction::F;
    if (dirCh == 'UB')
        return Direction::UB;
    if (dirCh == 'UF')
        return Direction::UF;
    if (dirCh == 'DB')
        return Direction::DB;
    if (dirCh == 'DF')
        return Direction::DF;

    return Direction::N; // Default to neutral
}

Button CommandParser::parseButton(const char buttonCh)
{
    if (buttonCh == 'x')
        return Button::x;
    if (buttonCh == 'y')
        return Button::y;
    if (buttonCh == 'z')
        return Button::z;
    if (buttonCh == 'a')
        return Button::a;
    if (buttonCh == 'b')
        return Button::b;
    if (buttonCh == 'c')
        return Button::c;
    if (buttonCh == 's')
        return Button::s;
    if (buttonCh == 'o')
        return Button::o;

    return Button::n; // Default to no button
}

std::string CommandParser::trim(const std::string &str)
{
    size_t first = str.find_first_not_of(" \t");
    if (first == std::string::npos)
        return "";

    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}

std::vector<std::string> CommandParser::split(const std::string &str, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);

    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }

    return tokens;
}