#pragma once
#include "Asik.h"
#include "Throw.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

/// @brief Represents a player: name, collected polymorphic Asiks, and points.
class Player {
private:
    std::string name;
    std::vector<std::unique_ptr<Asik>> collected;
    int points = 0;

    /// @brief Sum of Asik values (not the effect points).
    int calcAsikPoints() const;

public:
    explicit Player(const std::string& n);

    /// @brief Deep-copy constructor using clone().
    Player(const Player& o);

    /// @brief Deep-copy assignment using clone().
    Player& operator=(const Player& o);

    Player(Player&&) noexcept = default;
    Player& operator=(Player&&) noexcept = default;

    /// @brief Adds points (can be negative).
    void addPoints(int delta);

    /// @brief Collects a polymorphic Asik (deep-copy) and applies its effect.
    void collectAsik(const Asik& a);

    /// @brief Simulates a throw and updates the player's state.
    void makeThrow();

    /// @brief Total score: base points + sum of Asik values.
    int getTotalScore() const;

    const std::string& getName() const;

    friend std::ostream& operator<<(std::ostream& os, const Player& p);
};
