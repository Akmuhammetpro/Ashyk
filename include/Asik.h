#pragma once
#include <iostream>
#include <random>
#include <memory>

class Player; // вперёд объявляем, чтобы можно было ссылаться

class Asik {
protected:
    int value;

public:
    explicit Asik(int val = 1);
    virtual ~Asik() = default;

    void roll();              // как раньше: случайное значение 1..4
    int getValue() const;

    // ВИРТУАЛЬНАЯ ФУНКЦИЯ, специфичная для игры
    // по умолчанию: +10 пунктов игроку
    virtual void applyEffect(Player& j) const;

    // "виртуальный конструктор" (clone)
    virtual std::unique_ptr<Asik> clone() const;

    // NVI: внешний интерфейс НЕ virtual
    void print(std::ostream& os) const;

protected:
    // детальная печать — virtual
    virtual void printDetails(std::ostream& os) const;

    friend std::ostream& operator<<(std::ostream& os, const Asik& a);
};

// ===== Наследник 1: бонусный ашык =====
class BonusAsik : public Asik {
public:
    explicit BonusAsik(int val = 1);

    void applyEffect(Player& j) const override;      // даёт больше очков
    std::unique_ptr<Asik> clone() const override;

protected:
    void printDetails(std::ostream& os) const override;
};

// ===== Наследник 2: штрафной ашык =====
class PenaltyAsik : public Asik {
public:
    explicit PenaltyAsik(int val = 1);

    void applyEffect(Player& j) const override;      // снимает очки
    std::unique_ptr<Asik> clone() const override;

protected:
    void printDetails(std::ostream& os) const override;
};
