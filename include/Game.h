#pragma once
#include "Player.h"
#include "Errors.h"
#include <vector>
#include <iostream>

class Game {
private:
    std::vector<Player> players;
    int round = 0;

    // static атрибут: максимум разрешённых раундов
    static int maxRounds;

public:
    explicit Game(const std::vector<std::string>& names);
    void playRound();
    Player& getWinner();
    // static функции доступа к maxRounds
    static void setMaxRounds(int n);   // может бросить InvalidConfigError
    static int getMaxRounds();

    friend std::ostream& operator<<(std::ostream& os, const Game& g);

    const std::vector<Player>& getPlayers() const { return players; }
    int getRound() const { return round; }

    friend std::ostream& operator<<(std::ostream& os, const Game& g);
};
