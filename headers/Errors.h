#pragma once
#include <stdexcept>
#include <string>

/// @brief Base class for all game-specific errors.
class GameError : public std::runtime_error {
public:
    /// @brief Constructs a game error with a human-readable message.
    explicit GameError(const std::string& msg)
        : std::runtime_error(msg) {}
};

/// @brief Thrown when the number of players is invalid.
class InvalidPlayerCountError : public GameError {
public:
    InvalidPlayerCountError()
        : GameError("At least one player is required.") {}
};

/// @brief Thrown when game configuration is invalid (e.g., bad maxRounds).
class InvalidConfigError : public GameError {
public:
    /// @param msg Details about the invalid configuration.
    explicit InvalidConfigError(const std::string& msg)
        : GameError("Invalid configuration: " + msg) {}
};

/// @brief Thrown when a throw distance is invalid (e.g., negative).
class InvalidThrowDistanceError : public GameError {
public:
    InvalidThrowDistanceError()
        : GameError("Throw distance cannot be negative.") {}
};

/// @brief Thrown when trying to play more rounds than allowed by configuration.
class MaxRoundsReachedError : public GameError {
public:
    MaxRoundsReachedError()
        : GameError("Maximum number of rounds has been reached.") {}
};
