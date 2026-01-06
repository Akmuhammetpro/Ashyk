#pragma once
#include <iostream>
#include <memory>
#include <random>
#include <string>

class Player;  // forward declaration

/// @brief Base class for an "Așik" (knucklebone).
///        Supports polymorphic effects, cloning, and virtual printing (NVI).
class Asik {
protected:
    int value;

public:
    /// @brief Constructs an Asik with a given value (clamped to [1..4]).
    explicit Asik(int val = 1);
    virtual ~Asik() = default;

    /// @brief Rolls the Asik and sets a random value in [1..4].
    void roll();

    /// @brief Returns current Asik value.
    int getValue() const;

    /// @brief Applies this Asik effect to the player (game-specific virtual behavior).
    ///        Default: +10 points.
    virtual void applyEffect(Player& player) const;

    /// @brief Polymorphic clone ("virtual constructor").
    virtual std::unique_ptr<Asik> clone() const;

    /// @brief Returns a short readable type name (used for UI/logging).
    virtual const char* kindName() const;

    /// @brief Non-virtual print interface (NVI).
    void print(std::ostream& os) const;

protected:
    /// @brief Detailed printing (virtual).
    virtual void printDetails(std::ostream& os) const;

    /// @brief Stream output.
    friend std::ostream& operator<<(std::ostream& os, const Asik& a);
};

/// @brief Bonus Asik: gives extra points.
class BonusAsik : public Asik {
public:
    explicit BonusAsik(int val = 1);

    void applyEffect(Player& player) const override;
    std::unique_ptr<Asik> clone() const override;
    const char* kindName() const override;

protected:
    void printDetails(std::ostream& os) const override;
};

/// @brief Penalty Asik: subtracts points.
class PenaltyAsik : public Asik {
public:
    explicit PenaltyAsik(int val = 1);

    void applyEffect(Player& player) const override;
    std::unique_ptr<Asik> clone() const override;
    const char* kindName() const override;

protected:
    void printDetails(std::ostream& os) const override;
};

/// @brief Golden Asik: special scoring based on value.
class GoldenAsik : public Asik {
public:
    explicit GoldenAsik(int val = 1);

    void applyEffect(Player& player) const override;
    std::unique_ptr<Asik> clone() const override;
    const char* kindName() const override;

protected:
    void printDetails(std::ostream& os) const override;
};
