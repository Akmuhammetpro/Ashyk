#include "Game.h"
#include "Errors.h"
#include <algorithm>
#include <iostream>

/// Static field initialization
int Game::maxRounds = 10;

Game::Game(const std::vector<std::string>& names) {
    if (names.empty()) {
        // Example: throw from constructor
        throw InvalidPlayerCountError();
    }

    players.reserve(names.size());
    for (const auto& n : names) {
        players.emplace_back(n);
    }
}

void Game::playRound() {
    if (round >= maxRounds) {
        throw MaxRoundsReachedError();
    }

    ++round;
    std::cout << "\n=== Round " << round << " ===\n";

    for (auto& p : players) {
        p.makeThrow();
    }
}

Player& Game::getWinner() {
    return *std::max_element(
        players.begin(),
        players.end(),
        [](const Player& a, const Player& b) {
            return a.getTotalScore() < b.getTotalScore();
        });
}

void Game::setMaxRounds(int n) {
    if (n <= 0) {
        throw InvalidConfigError("Number of rounds must be positive.");
    }
    maxRounds = n;
}

int Game::getMaxRounds() {
    return maxRounds;
}

std::ostream& operator<<(std::ostream& os, const Game& g) {
    os << "Game (round " << g.round << " / max " << Game::getMaxRounds() << "):\n";
    for (const auto& p : g.players) {
        os << "  " << p << "\n";
    }
    return os;
}
