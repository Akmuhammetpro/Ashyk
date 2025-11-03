#include "Game.h"
#include <algorithm>

Game::Game(const std::vector<std::string>& names) {
    for (const auto& n : names)
        players.emplace_back(n);
}

void Game::playRound() {
    ++round;
    std::cout << "\n=== Round " << round << " ===\n";
    for (auto& p : players)
        p.makeThrow();
}

Player& Game::getWinner() {
    return *std::max_element(players.begin(), players.end(),
        [](const Player& a, const Player& b) {
            return a.getTotalScore() < b.getTotalScore();
        });
}

std::ostream& operator<<(std::ostream& os, const Game& g) {
    os << "Game (round " << g.round << "):\n";
    for (const auto& p : g.players)
        os << "  " << p << "\n";
    return os;
}
