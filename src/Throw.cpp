#include "Throw.h"
#include <random>

Throw::Throw(std::unique_ptr<Asik> k, double d)
    : kenek(std::move(k)), distance(d) {}

bool Throw::isSuccessful() const {
    return distance >= 3.0;
}

Throw Throw::simulate() {
    // решаем, какой тип ашыка создать
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> type_dis(0, 2);  // 0 = обычный, 1 = бонус, 2 = штраф
    std::uniform_int_distribution<> value_dis(1, 4);
    std::uniform_real_distribution<> dist_dis(0.0, 5.0);

    int val = value_dis(gen);
    int type = type_dis(gen);

    std::unique_ptr<Asik> p;

    if (type == 0) {
        p = std::make_unique<Asik>(val);          // обычный
    } else if (type == 1) {
        p = std::make_unique<BonusAsik>(val);     // бонусный
    } else {
        p = std::make_unique<PenaltyAsik>(val);   // штрафной
    }

    double d = dist_dis(gen);
    return Throw(std::move(p), d);
}

const Asik& Throw::getKenek() const {
    return *kenek;  // возвращаем ссылку на базовый Asik
}

double Throw::getDistance() const {
    return distance;
}

std::ostream& operator<<(std::ostream& os, const Throw& t) {
    os << "Throw(dist=" << t.distance << " talpe, " << *t.kenek << ")";
    return os;
}
