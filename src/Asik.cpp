#include "Asik.h"
#include "Player.h"
#include <random>

Asik::Asik(int val) : value(val) {
    if (val < 1 || val > 4) {
        value = 1;
    }
}

// cppcheck-suppress unusedFunction   // used from Throw::simulate (other translation unit)
void Asik::roll() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 4);
    value = dis(gen);
}

int Asik::getValue() const {
    return value;
}

void Asik::applyEffect(Player& player) const {
    // default behavior: +10 points
    player.addPoints(10);
}

std::unique_ptr<Asik> Asik::clone() const {
    return std::make_unique<Asik>(*this);
}

const char* Asik::kindName() const {
    return "Normal";
}

void Asik::printDetails(std::ostream& os) const {
    os << "Asik(" << value << ")";
}

void Asik::print(std::ostream& os) const {
    printDetails(os);  // virtual dispatch happens here
}

std::ostream& operator<<(std::ostream& os, const Asik& a) {
    a.print(os);
    return os;
}

// ===== BonusAsik =====

BonusAsik::BonusAsik(int val) : Asik(val) {}

void BonusAsik::applyEffect(Player& player) const {
    player.addPoints(20);
}

std::unique_ptr<Asik> BonusAsik::clone() const {
    return std::make_unique<BonusAsik>(*this);
}

const char* BonusAsik::kindName() const {
    return "Bonus";
}

void BonusAsik::printDetails(std::ostream& os) const {
    os << "BonusAsik(" << value << ")";
}

// ===== PenaltyAsik =====

PenaltyAsik::PenaltyAsik(int val) : Asik(val) {}

void PenaltyAsik::applyEffect(Player& player) const {
    player.addPoints(-10);
}

std::unique_ptr<Asik> PenaltyAsik::clone() const {
    return std::make_unique<PenaltyAsik>(*this);
}

const char* PenaltyAsik::kindName() const {
    return "Penalty";
}

void PenaltyAsik::printDetails(std::ostream& os) const {
    os << "PenaltyAsik(" << value << ")";
}

// ===== GoldenAsik =====

GoldenAsik::GoldenAsik(int val) : Asik(val) {}

void GoldenAsik::applyEffect(Player& player) const {
    player.addPoints(value * 5);
}

std::unique_ptr<Asik> GoldenAsik::clone() const {
    return std::make_unique<GoldenAsik>(*this);
}

const char* GoldenAsik::kindName() const {
    return "Golden";
}

void GoldenAsik::printDetails(std::ostream& os) const {
    os << "GoldenAsik(" << value << ")";
}
