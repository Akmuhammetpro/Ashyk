#include "GameEngine.h"
#include "RaylibGameEngine.h"
#include <iostream>

int main() {
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif
    std::cout << "\n=== AŞYK ===\n";

    try {
        // Вариант 1: графика
        #ifdef GITHUB_ACTIONS
        GameEngine engine;
        engine.run();
        #else
        RaylibGameEngine engine;
        engine.Run();
        #endif

        // Вариант 2: консоль (если нужно)
        // GameEngine engine;
        // engine.run();
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
