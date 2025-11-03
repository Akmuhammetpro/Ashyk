#pragma once
#include <iostream>
#include <random>

class Asik {
private:
    int value;
public:
    explicit Asik(int val = 1);
    void roll();
    int getValue() const;
    friend std::ostream& operator<<(std::ostream& os, const Asik& a);
};
