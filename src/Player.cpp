#include "Player.h"
#include <algorithm>

int Player::calcAsikPoints() const {
    int sum = 0;
    for (const auto& a : collected)
        sum += a.getValue();
    return sum;
}

Player::Player(const std::string& n) : name(n) {}
Player::Player(const Player& o) : name(o.name), collected(o.collected), points(o.points) {}

Player& Player::operator=(Player o) {
    swap(*this, o);
    return *this;
}

void swap(Player& a, Player& b) noexcept {
    using std::swap;
    swap(a.name, b.name);
    swap(a.collected, b.collected);
    swap(a.points, b.points);
}

void Player::collectAsik(const Asik& a) {
    collected.push_back(a);
    points += 10;
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
        os << p.collected[i].getValue();
    }
    os << "], Total:" << p.getTotalScore() << ")";
    return os;
}
