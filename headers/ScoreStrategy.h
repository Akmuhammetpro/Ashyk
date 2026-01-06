#pragma once
#include "Player.h"
#include <memory>

/// @brief Strategy interface for computing a player's score.
///        Used by the Raylib UI to show alternative scoring rules.
class ScoreStrategy {
public:
    virtual ~ScoreStrategy() = default;

    /// @brief Computes the score for the given player using a specific rule.
    virtual int compute(const Player& p) const = 0;

    /// @brief Human-readable strategy name (for UI).
    virtual const char* getName() const = 0;
};

/// @brief Classic scoring: just Player::getTotalScore().
class ClassicScoreStrategy final : public ScoreStrategy {
public:
    int compute(const Player& p) const override {
        return p.getTotalScore();
    }

    const char* getName() const override {
        return "Classic";
    }
};

/// @brief Double scoring: 2x Player::getTotalScore().
class DoubleScoreStrategy final : public ScoreStrategy {
public:
    int compute(const Player& p) const override {
        return p.getTotalScore() * 2;
    }

    const char* getName() const override {
        return "Double x2";
    }
};

/// @brief Hardcore mode: subtract 20 points, but never below 0.
class HardcorePenaltyStrategy final : public ScoreStrategy {
public:
    int compute(const Player& p) const override {
        const int v = p.getTotalScore() - 20;
        return (v < 0) ? 0 : v;
    }

    const char* getName() const override {
        return "Hardcore -20";
    }
};