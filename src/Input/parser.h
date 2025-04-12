#pragma once

#include "input.h"
#include "nlohmann/json.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using json = nlohmann::json;

class CommandParser
{
public:
    CommandParser(int targetFPS);
    ~CommandParser() = default;

    // Parse a command file and add commands to the provided Input instance
    bool parseCommandFile(const std::string &characterName, Input &input);

private:
    // Parse a single command section
    CommandDefinition parseCommandSection(const json &command);

    // Parse a command string into a sequence of InputSymbols
    std::vector<InputSymbol> parseCommandSequence(const std::string &commandStr);

    // Parse a single input symbol
    InputSymbol parseInputSymbol(const std::string &symbolStr);

    // Convert a direction string to Direction enum
    Direction parseDirection(const std::string &dirStr);

    // Convert a button string to Button enum
    Button parseButton(const std::string &buttonStr);

    // Helper function to trim whitespace from strings
    std::string trim(const std::string &str);

    // Split a string by a delimiter
    std::vector<std::string> split(const std::string &str, char delimiter);

    const int m_targetFPS;
};
