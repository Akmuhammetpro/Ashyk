#include "Asik.h"
#include "Player.h"   // чтобы видеть Player::addPoints
#include <random>

Asik::Asik(int val) : value(val) {
    if (val < 1 || val > 4) value = 1;
}

void Asik::roll() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 4);
    value = dis(gen);
}

int Asik::getValue() const {
    return value;
}

// базовый эффект: +10 пунктов (как раньше ты делал в collectAsik)
void Asik::applyEffect(Player& j) const {
    j.addPoints(10);
}

// базовый clone: копируем как Asik
std::unique_ptr<Asik> Asik::clone() const {
    return std::make_unique<Asik>(*this);
}

// базовая детальная печать
void Asik::printDetails(std::ostream& os) const {
    os << "Asik(" << value << ")";
}

// NVI: не virtual
void Asik::print(std::ostream& os) const {
    printDetails(os); // вызовет верную версию в наследнике
}

std::ostream& operator<<(std::ostream& os, const Asik& a) {
    a.print(os);
    return os;
}

// ===== BonusAsik =====

BonusAsik::BonusAsik(int val) : Asik(val) {}

void BonusAsik::applyEffect(Player& j) const {
    // например, +20 вместо +10
    j.addPoints(20);
}

std::unique_ptr<Asik> BonusAsik::clone() const {
    return std::make_unique<BonusAsik>(*this);
}

void BonusAsik::printDetails(std::ostream& os) const {
    os << "BonusAsik(" << value << ")";
}

// ===== PenaltyAsik =====

PenaltyAsik::PenaltyAsik(int val) : Asik(val) {}

void PenaltyAsik::applyEffect(Player& j) const {
    // штраф, например -10 пунктов
    j.addPoints(-10);
}

std::unique_ptr<Asik> PenaltyAsik::clone() const {
    return std::make_unique<PenaltyAsik>(*this);
}

void PenaltyAsik::printDetails(std::ostream& os) const {
    os << "PenaltyAsik(" << value << ")";
}
// ===== GoldenAsik =====

GoldenAsik::GoldenAsik(int val) : Asik(val) {}

void GoldenAsik::applyEffect(Player& j) const {
    // супер-бонус: например, value * 5 пунктов
    j.addPoints(value * 5);
}

std::unique_ptr<Asik> GoldenAsik::clone() const {
    return std::make_unique<GoldenAsik>(*this);
}

void GoldenAsik::printDetails(std::ostream& os) const {
    os << "GoldenAsik(" << value << ")";
}
