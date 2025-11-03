#pragma once
#include "Player.h"
#include <vector>
#include <iostream>

class Game {
private:
    std::vector<Player> players;
    int round = 0;
public:
    Game(const std::vector<std::string>& names);
    void playRound();
    Player& getWinner();
    friend std::ostream& operator<<(std::ostream& os, const Game& g);
};
