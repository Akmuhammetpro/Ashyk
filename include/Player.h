#pragma once
#include "Asik.h"
#include "Throw.h"
#include <vector>
#include <string>
#include <iostream>
#include <memory>

class Player {
private:
    std::string name;
    std::vector<std::unique_ptr<Asik>> collected;
    int points = 0;

    int calcAsikPoints() const;

public:
    explicit Player(const std::string& n);

    // Копирующий конструктор и operator= с deep-copy через clone()
    Player(const Player& o);
    Player& operator=(const Player& o);

    // move-конструктор/оператор можно оставить по умолчанию
    Player(Player&&) noexcept = default;
    Player& operator=(Player&&) noexcept = default;

    // даём Asik-ам возможность менять очки
    void addPoints(int delta);

    void collectAsik(const Asik& a);
    void makeThrow();
    int getTotalScore() const;
    const std::string& getName() const;

    friend std::ostream& operator<<(std::ostream& os, const Player& p);
};
