#include "RaylibGameEngine.h"
#include "raylib.h"

#include <string>
#include <vector>

RaylibGameEngine::RaylibGameEngine()
    : game_({ "Andy", "Mehmet", "Anna" }) {
    // Если тебе надо читать конфиг/игроков — сделай в InitGameFromConfigOrDefaults()
}

void RaylibGameEngine::InitGameFromConfigOrDefaults() {
    // ВАЖНО: тут зависит от твоего Game API.
    // Примеры (выбери что подходит):
    // game_.SetupPlayers({"A", "B"});
    // game_.LoadConfig("config.txt");
    // game_.Start();
}

// cppcheck-suppress unusedFunction
void RaylibGameEngine::Run() {
    InitWindow(1280, 720, "Ashyk (Raylib)");
    SetTargetFPS(60);

    ChangeDirectory(GetApplicationDirectory());

    InitGameFromConfigOrDefaults();

    const char* spritePath = "assets/player/throw_strip.png";
    if (!FileExists(spritePath)) {
        TraceLog(LOG_ERROR, "Missing sprite: %s", spritePath);
    }

    playerAnimator_.Load(spritePath, 5, 12.0f);
    playerAnimator_.SetPosition(Vector2{ 40, 260 });
    playerAnimator_.SetReleaseFrame(2);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        HandleInput();
        Update(dt);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        Draw();
        EndDrawing();
    }

    playerAnimator_.Unload();
    CloseWindow();
}

void RaylibGameEngine::HandleInput() {
    // Пример: SPACE запускает бросок
    if (IsKeyPressed(KEY_SPACE)) {
        // стартуем анимацию
        playerAnimator_.StartThrow();
    }
}

void RaylibGameEngine::Update(float deltaTime) {
    // Если анимация сказала "кадр выпуска наступил" — делаем реальный бросок
    if (playerAnimator_.Update(deltaTime)) {
        DoThrow();
    }
}

void RaylibGameEngine::DoThrow() {
    // === ВАЖНО: тут подключаешь твою реальную логику ===
    // Т.к. я не вижу твой Game API, оставляю шаблон:

    // Вариант A: если есть "текущий игрок"
    // game_.GetCurrentPlayer().makeThrow();

    // Вариант B: если Game сам обрабатывает бросок
    // game_.MakeThrow();

    // Вариант C: если бросок связан с меню/раундом
    // game_.PlayTurn();

    // Пока — заглушка (удали, когда подключишь реальную функцию)
    // (Можно вывести в консоль)
    // std::cout << "Throw triggered on animation frame!\n";
}

void RaylibGameEngine::Draw() {
    DrawText("SPACE = throw animation", 20, 20, 20, DARKGRAY);

    // Рисуем игрока
    playerAnimator_.Draw();

    // Тут позже: фон, стол, ашики, UI очков и т.д.
}
