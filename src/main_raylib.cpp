#include "raylib.h"

#include "Errors.h"
#include "Game.h"
#include "ScoreStrategy.h"

#include <memory>
#include <string>
#include <vector>

int main() {
    const int initialW = 1280;
    const int initialH = 720;

    InitWindow(initialW, initialH, "Ashyk - Raylib");
    SetTargetFPS(60);

    // --- GAME SETUP ---
    std::vector<std::string> names = {"Andy", "Mehmet", "Anna"};
    Game::setMaxRounds(10);
    Game game(names);

    std::string status = "Press SPACE to play a round";

    // --- SCORE STRATEGY ---
    std::unique_ptr<ScoreStrategy> strategy = std::make_unique<ClassicScoreStrategy>();

    // --- ASIK MOVEMENT ---
    const float asikRadius = 30.0f;
    float asikX = initialW * 0.15f;
    float asikY = initialH * 0.7f;
    const float asikSpeed = 600.0f;
    bool asikMoving = false;

    while (!WindowShouldClose()) {
        const float dt = GetFrameTime();
        const int w = GetScreenWidth();
        const int h = GetScreenHeight();

        // INPUT
        if (IsKeyPressed(KEY_SPACE)) {
            try {
                game.playRound();
                status = "Round played!";
                asikX = w * 0.15f;
                asikY = h * 0.7f;
                asikMoving = true;
            } catch (const GameError& e) {
                status = e.what();
                asikMoving = false;
            } catch (const std::exception& e) {
                status = std::string("Unexpected error: ") + e.what();
                asikMoving = false;
            }
        }

        // Strategy switch
        if (IsKeyPressed(KEY_ONE)) {
            strategy = std::make_unique<ClassicScoreStrategy>();
            status = "Strategy: Classic";
        }
        if (IsKeyPressed(KEY_TWO)) {
            strategy = std::make_unique<DoubleScoreStrategy>();
            status = "Strategy: Double x2";
        }
        if (IsKeyPressed(KEY_THREE)) {
            strategy = std::make_unique<HardcorePenaltyStrategy>();
            status = "Strategy: Hardcore -20";
        }

        // Fullscreen toggle
        if (IsKeyPressed(KEY_F)) {
            ToggleFullscreen();
        }

        // Movement update
        if (asikMoving) {
            asikX += asikSpeed * dt;
            if (asikX > w * 0.85f) {
                asikMoving = false;
            }
        }

        // DRAW
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("ASHYK - graphical version", 40, 20, 30, BLACK);
        DrawText(status.c_str(), 40, 60, 20, DARKGRAY);

        const std::string roundText = "Round: " + std::to_string(game.getRound());
        DrawText(roundText.c_str(), 40, 90, 20, DARKBLUE);

        const std::string stratName = std::string("Current strategy: ") + strategy->getName();
        DrawText(stratName.c_str(), 40, 120, 20, DARKBLUE);

        int y = 160;
        for (const auto& p : game.getPlayers()) {
            const int baseScore = p.getTotalScore();
            const int stratScore = strategy->compute(p);

            const std::string line = p.getName() +
                                     " : base=" + std::to_string(baseScore) +
                                     ", strategy=" + std::to_string(stratScore);

            DrawText(line.c_str(), 40, y, 20, BLACK);
            y += 28;
        }

        DrawText("SPACE - play a round", 40, h - 90, 18, DARKGRAY);
        DrawText("1/2/3 - change score strategy", 40, h - 65, 18, DARKGRAY);
        DrawText("F - fullscreen ON/OFF, ESC - exit", 40, h - 40, 18, DARKGRAY);

        const float groundY = h * 0.75f;
        DrawLine(0, (int)groundY, w, (int)groundY, DARKGRAY);

        DrawCircle((int)asikX, (int)asikY, asikRadius, RED);
        DrawCircleLines((int)asikX, (int)asikY, asikRadius, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
