#include "Player.h"
#include <algorithm>

int Player::calcAsikPoints() const {
    int sum = 0;
    for (const auto& a : collected)
        sum += a->getValue();     // теперь a — unique_ptr<Asik>
    return sum;
}

Player::Player(const std::string& n) : name(n) {}

// Копирующий конструктор: deep-copy с clone()
Player::Player(const Player& o) : name(o.name), points(o.points) {
    collected.clear();
    collected.reserve(o.collected.size());
    for (const auto& a : o.collected) {
        if (a) {
            collected.push_back(a->clone()); // полиморфное копирование
        }
    }
}

// operator= с deep-copy
Player& Player::operator=(const Player& o) {
    if (this == &o) return *this;

    name = o.name;
    points = o.points;

    collected.clear();
    collected.reserve(o.collected.size());
    for (const auto& a : o.collected) {
        if (a) {
            collected.push_back(a->clone());
        }
    }

    return *this;
}

void Player::addPoints(int delta) {
    points += delta;
}

void Player::collectAsik(const Asik& a) {
    collected.push_back(a.clone());

    if (dynamic_cast<const BonusAsik*>(&a)) {
        std::cout << "  [Bonus asyk collected!]\n";
    } else if (dynamic_cast<const PenaltyAsik*>(&a)) {
        std::cout << "  [Penalty asyk collected...]\n";
    } else if (dynamic_cast<const GoldenAsik*>(&a)) {
        std::cout << "  [Golden asyk collected!!!]\n";
    } else {
        std::cout << "  [Normal asyk collected]\n";
    }

    a.applyEffect(*this);
}



void Player::makeThrow() {
    Throw t = Throw::simulate();
    std::cout << name << " throws: " << t;
    if (t.isSuccessful()) {
        std::cout << " → Yes! Got it!\n";
        collectAsik(t.getKenek());
    } else {
        std::cout << " → Miss the shot.\n";
    }
}

int Player::getTotalScore() const {
    return points + calcAsikPoints();
}

const std::string& Player::getName() const {
    return name;
}

std::ostream& operator<<(std::ostream& os, const Player& p) {
    os << p.name << " (points:" << p.points << ", ashyks:[";
    for (size_t i = 0; i < p.collected.size(); ++i) {
        if (i > 0) os << ",";
        os << p.collected[i]->getValue();
    }
    os << "], Total:" << p.getTotalScore() << ")";
    return os;
}
