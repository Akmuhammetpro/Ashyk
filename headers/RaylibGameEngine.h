#pragma once
#include "Game.h"
#include "PlayerAnimation.h"

class RaylibGameEngine {
public:
    RaylibGameEngine();
    void Run();

private:
    Game game_;
    PlayerAnimator playerAnimator_;

    void InitGameFromConfigOrDefaults();
    void HandleInput();
    void Update(float deltaTime);
    void Draw();

    // Здесь мы вызываем твою игровую логику броска
    void DoThrow();
};
