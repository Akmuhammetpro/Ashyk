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
        int maxR = readInt("Introduceti numarul maxim de runde (ex: 5): ");
        try {
            Game::setMaxRounds(maxR);
            break;
        } catch (const InvalidConfigError& e) {
            std::cerr << "[Configuratie] " << e.what() << "\n";
        }
    }

    cout << "Jocul a fost configurat cu succes.\n";
}

vector<string> GameEngine::readPlayerNames() const {
    int n = readInt("Introduceti numarul de jucatori (>=1): ");
    if (n <= 0) {
        throw InvalidPlayerCountError();
    }

    vector<string> names;
    names.reserve(n);

    cout << "Introduceti numele jucatorilor:\n";
    for (int i = 0; i < n; ++i) {
        cout << "  Jucator " << (i + 1) << ": ";
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
        cout << "Valoare invalida, incercati din nou.\n";
    }
}

void GameEngine::printMenu() const {
    cout << "\n======= MENIU JOC AȘYK =======\n";
    cout << "1. Joaca o runda\n";
    cout << "2. Joaca toate rundele ramase pana la maxim\n";
    cout << "3. Afiseaza scorurile curente\n";
    cout << "4. Afiseaza castigatorul si iesi\n";
    cout << "0. Iesire fara castigator\n";
    cout << "Alegeti o optiune: ";
}

void GameEngine::handleChoice(int choice) {
    switch (choice) {
    case 1:
        game.playRound();
        break;

    case 2: {
        // Play until the max round limit is reached
        while (true) {
            try {
                game.playRound();
            } catch (const MaxRoundsReachedError& e) {
                std::cerr << "[Limita runde] " << e.what() << "\n";
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
        cout << "Castigator: " << game.getWinner().getName()
             << " — " << game.getWinner().getTotalScore() << " puncte!\n";
        running = false;
        break;

    case 0:
        running = false;
        break;

    default:
        cout << "Optiune invalida.\n";
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
            cout << "Valoare invalida.\n";
            continue;
        }

        try {
            handleChoice(choice);
        } catch (const InvalidConfigError& e) {
            std::cerr << "[Configuratie invalida] " << e.what() << "\n";
        } catch (const InvalidPlayerCountError& e) {
            std::cerr << "[Jucatori invalizi] " << e.what() << "\n";
        } catch (const GameError& e) {
            std::cerr << "[Eroare de joc] " << e.what() << "\n";
        } catch (const std::exception& e) {
            std::cerr << "[Eroare neasteptata] " << e.what() << "\n";
        }
    }

    cout << "\nIesire din joc.\n";
}
