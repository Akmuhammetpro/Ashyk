#include "GameEngine.h"
#include <iostream>

using std::cin;
using std::cout;
using std::string;
using std::vector;

GameEngine::GameEngine()
    : game(readPlayerNames()) // create game after reading player names
{
    // Configure max rounds with validation
    while (true) {
        int maxR = readInt("Enter the maximum number of rounds (e.g., 5): ");
        try {
            Game::setMaxRounds(maxR);
            break;
        } catch (const InvalidConfigError& e) {
            std::cerr << "[Config] " << e.what() << "\n";
        }
    }

    cout << "Game configured successfully.\n";
}

vector<string> GameEngine::readPlayerNames() const {
    int n = readInt("Enter number of players (>= 1): ");
    if (n <= 0) {
        throw InvalidPlayerCountError();
    }

    vector<string> names;
    names.reserve(n);

    cout << "Enter player names:\n";
    for (int i = 0; i < n; ++i) {
        cout << "  Player " << (i + 1) << ": ";
        string name;
        cin >> name;
        names.push_back(name);
    }

    return names;
}

int GameEngine::readInt(const string& prompt) const {
    while (true) {
        cout << prompt;
        int x;
        if (cin >> x) {
            return x;
        }
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid value, try again.\n";
    }
}

void GameEngine::printMenu() const {
    cout << "\n======= ASHYK MENU =======\n";
    cout << "1. Play one round\n";
    cout << "2. Play all remaining rounds until max\n";
    cout << "3. Show current scores\n";
    cout << "4. Show winner and exit\n";
    cout << "0. Exit\n";
    cout << "Choose an option: ";
}

void GameEngine::handleChoice(int choice) {
    switch (choice) {
    case 1:
        game.playRound();
        break;

    case 2: {
        while (true) {
            try {
                game.playRound();
            } catch (const MaxRoundsReachedError& e) {
                std::cerr << "[Round limit] " << e.what() << "\n";
                break;
            }
        }
        break;
    }

    case 3:
        cout << "\n" << game << "\n";
        break;

    case 4:
        cout << "\n" << game << "\n";
        cout << "Winner: " << game.getWinner().getName()
             << " — " << game.getWinner().getTotalScore() << " points!\n";
        running = false;
        break;

    case 0:
        running = false;
        break;

    default:
        cout << "Invalid option.\n";
        break;
    }
}

void GameEngine::run() {
    while (running) {
        printMenu();

        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid value.\n";
            continue;
        }

        try {
            handleChoice(choice);
        } catch (const GameError& e) {
            std::cerr << "[Game error] " << e.what() << "\n";
        } catch (const std::exception& e) {
            std::cerr << "[Unexpected error] " << e.what() << "\n";
        }
    }

    cout << "\nExiting game.\n";
}