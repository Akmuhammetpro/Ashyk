#include "Throw.h"
#include <random>

Throw::Throw(std::unique_ptr<Asik> k, double d)
    : kenek(std::move(k)), distance(d) {}

bool Throw::isSuccessful() const {
    return distance >= 3.0;
}

Throw Throw::simulate() {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> typeDis(0, 3);   // 0=Normal, 1=Bonus, 2=Penalty, 3=Golden
    std::uniform_int_distribution<> valueDis(1, 4);
    std::uniform_real_distribution<> distDis(0.0, 5.0);

    const int val = valueDis(gen);
    const int type = typeDis(gen);

    std::unique_ptr<Asik> p;
    switch (type) {
    case 0: p = std::make_unique<Asik>(val); break;
    case 1: p = std::make_unique<BonusAsik>(val); break;
    case 2: p = std::make_unique<PenaltyAsik>(val); break;
    default: p = std::make_unique<GoldenAsik>(val); break;
    }

    return Throw(std::move(p), distDis(gen));
}

const Asik& Throw::getKenek() const {
    return *kenek;
}

// cppcheck-suppress unusedFunction   // kept for future UI/logic, not used in this TU
double Throw::getDistance() const {
    return distance;
}

std::ostream& operator<<(std::ostream& os, const Throw& t) {
    os << "Throw(dist=" << t.distance << " talpe, " << *t.kenek << ")";
    return os;
}
