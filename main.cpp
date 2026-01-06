#include "GameEngine.h"
#include <iostream>

int main() {
    system("chcp 65001 > nul");
    std::cout << "\n=== AŞYK ===\n";

    try {
        GameEngine engine;
        engine.run();
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
