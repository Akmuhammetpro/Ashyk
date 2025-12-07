#include "Game.h"
#include "Errors.h"
#include <iostream>

int main() {
    system("chcp 65001 > nul");
    std::cout << "\n=== AŞYK ===\n";

    try {
        std::vector<std::string> names = {"Andy", "Mehmet", "Anna"};

        // пример использования static-функции
        Game::setMaxRounds(5);

        Game game(names);

        // high-level сценарий: играем 3 раунда
        for (int i = 0; i < 3; ++i) {
            game.playRound();
        }

        std::cout << "\n" << game << "\n";
        std::cout << "Winner: " << game.getWinner().getName()
                  << " — " << game.getWinner().getTotalScore() << " points!\n";
    } catch (const GameError& e) {
        std::cerr << "\n[Game Error] " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "\n[Unexpected Error] " << e.what() << "\n";
        return 1;
    }

    return 0;
}
