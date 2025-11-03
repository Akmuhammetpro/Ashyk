#include "Throw.h"
#include <random>

Throw::Throw(const Asik& k, double d) : kenek(k), distance(d) {}

bool Throw::isSuccessful() const {
    return distance >= 3.0;
}

Throw Throw::simulate() {
    Asik k; k.roll();
    std::random_device rd; std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 5.0);
    return Throw(k, dis(gen));
}

const Asik& Throw::getKenek() const {
    return kenek;
}

double Throw::getDistance() const {
    return distance;
}

std::ostream& operator<<(std::ostream& os, const Throw& t) {
    os << "Throw(dist=" << t.distance << " talpe, " << t.kenek << ")";
    return os;
}
