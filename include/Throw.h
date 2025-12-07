#pragma once
#include "Asik.h"
#include <memory>    // для std::unique_ptr

class Throw {
private:
    std::unique_ptr<Asik> kenek;  // теперь полиморфный ашык
    double distance;
public:
    Throw(std::unique_ptr<Asik> k, double d);

    bool isSuccessful() const;
    static Throw simulate();

    const Asik& getKenek() const; // вернём ссылку на базу
    double getDistance() const;

    friend std::ostream& operator<<(std::ostream& os, const Throw& t);
};
