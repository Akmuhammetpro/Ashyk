#pragma once
#include "Asik.h"

class Throw {
private:
    Asik kenek;
    double distance;
public:
    Throw(const Asik& k, double d);
    bool isSuccessful() const;
    static Throw simulate();
    const Asik& getKenek() const;
    double getDistance() const;
    friend std::ostream& operator<<(std::ostream& os, const Throw& t);
};
