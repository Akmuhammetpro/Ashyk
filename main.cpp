#include "Game.h"
#include "Errors.h"
#include <iostream>
#include <vector>

int main() {
    system("chcp 65001 > nul");
    std::cout << "\n=== AŞYK ===\n";

    try {
        std::vector<std::string> names = {"Andy", "Mehmet", "Anna"};

        // Example of using a static configuration function
        Game::setMaxRounds(5);

        Game game(names);

        // High-level scenario: play 3 rounds
        for (int i = 0; i < 3; ++i) {
            game.playRound();
        }

        std::cout << "\n" << game << "\n";
        std::cout << "Winner: " << game.getWinner().getName()
                  << " — " << game.getWinner().getTotalScore() << " points!\n";
    }
    catch (const InvalidConfigError& e) {
        std::cerr << "\n[Invalid Config] " << e.what() << "\n";
        return 1;
    }
    catch (const InvalidPlayerCountError& e) {
        std::cerr << "\n[Invalid Player Count] " << e.what() << "\n";
        return 1;
    }
    catch (const GameError& e) {
        std::cerr << "\n[Game Error] " << e.what() << "\n";
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "\n[Unexpected Error] " << e.what() << "\n";
        return 1;
    }

    return 0;
}
