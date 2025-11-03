#include "Game.h"
#include <iostream>

int main() {
    system("chcp 65001 > nul");
    std::cout << "\n=== AŞYK ===\n";
    std::vector<std::string> names = {"Andy", "Mehmet", "Anna"};
    Game game(names);
    game.playRound();
    game.playRound();
    game.playRound();
    std::cout << "\n" << game << "\n";
    std::cout << "Winner: " << game.getWinner().getName()
              << " — " << game.getWinner().getTotalScore() << " points!\n";
    return 0;
}
