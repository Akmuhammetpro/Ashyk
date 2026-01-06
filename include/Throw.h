#pragma once
#include "Asik.h"
#include <iostream>
#include <memory>

/// @brief Represents a throw attempt: a polymorphic Asik + a distance.
class Throw {
private:
    std::unique_ptr<Asik> kenek;
    double distance;

public:
    /// @brief Constructs a throw from an Asik pointer and a distance.
    Throw(std::unique_ptr<Asik> k, double d);

    /// @brief Returns true if the throw was successful.
    bool isSuccessful() const;

    /// @brief Simulates a random throw (random Asik type/value + random distance).
    static Throw simulate();

    /// @brief Returns the thrown Asik as base reference.
    const Asik& getKenek() const;

    /// @brief Returns distance.
    double getDistance() const;

    friend std::ostream& operator<<(std::ostream& os, const Throw& t);
};
