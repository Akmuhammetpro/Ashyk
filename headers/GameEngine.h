#pragma once

#include "Errors.h"
#include "Game.h"
#include <string>
#include <vector>

/// @brief Console application layer for the Ashyk game.
///        Handles input, menu loop, and calls Game methods.
class GameEngine {
private:
    Game game;              ///< The core game object
    bool running = true;    ///< Main loop flag

    /// @brief Reads player names from stdin.
    std::vector<std::string> readPlayerNames() const;

    /// @brief Reads an integer from stdin with validation (keeps asking until valid).
    int readInt(const std::string& prompt) const;

    /// @brief Prints the menu to stdout.
    void printMenu() const;

    /// @brief Executes a menu choice.
    void handleChoice(int choice);

public:
    /// @brief Creates the console engine, reads player setup, and configures the game.
    GameEngine();

    /// @brief Main menu loop.
    void run();
};
