#pragma once
#include "Asik.h"
#include "Throw.h"
#include <vector>
#include <string>
#include <iostream>

class Player {
private:
    std::string name;
    std::vector<Asik> collected;
    int points = 0;
    int calcAsikPoints() const;
public:
    explicit Player(const std::string& n);
    Player(const Player& o);
    Player& operator=(Player o);
    friend void swap(Player& a, Player& b) noexcept;
    void collectAsik(const Asik& a);
    void makeThrow();
    int getTotalScore() const;
    const std::string& getName() const;
    friend std::ostream& operator<<(std::ostream& os, const Player& p);
};
