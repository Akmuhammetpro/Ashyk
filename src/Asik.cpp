#include "Asik.h"

Asik::Asik(int val) : value(val) {
    if (val < 1 || val > 4) value = 1;
}

void Asik::roll() {
    std::random_device rd; std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 4);
    value = dis(gen);
}

int Asik::getValue() const {
    return value;
}

std::ostream& operator<<(std::ostream& os, const Asik& a) {
    return os << "Asik(" << a.value << ")";
}
