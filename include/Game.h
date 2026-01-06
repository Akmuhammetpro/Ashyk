#pragma once
#include "Errors.h"
#include "Player.h"
#include <iostream>
#include <vector>

/// @brief Controls the match: stores players, current round, and configuration.
class Game {
private:
    std::vector<Player> players;
    int round = 0;

    /// @brief Maximum allowed rounds.
    static int maxRounds;

public:
    /// @brief Creates a game from a list of player names.
    explicit Game(const std::vector<std::string>& names);

    /// @brief Plays one round (may throw MaxRoundsReachedError).
    void playRound();

    /// @brief Returns the player with the best total score.
    Player& getWinner();

    /// @brief Sets the maximum number of rounds (must be > 0).
    static void setMaxRounds(int n);   // may throw InvalidConfigError

    /// @brief Returns the maximum number of rounds.
    static int getMaxRounds();

    friend std::ostream& operator<<(std::ostream& os, const Game& g);

    // cppcheck-suppress unusedFunction   // used in graphical UI (main_raylib.cpp)
    const std::vector<Player>& getPlayers() const { return players; }

    // cppcheck-suppress unusedFunction   // used in graphical UI (main_raylib.cpp)
    int getRound() const { return round; }
};
