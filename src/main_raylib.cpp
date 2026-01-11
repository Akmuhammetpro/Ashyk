#include "raylib.h"

#include "Errors.h"
#include "Game.h"
#include "PlayerAnimation.h"
#include "ScoreStrategy.h"

#include <cmath>
#include <memory>
#include <string>
#include <vector>

static float GetThrowDistance() {
    return 260.0f; // tweak throw distance here
}

static float GetThrowFlightSeconds() {
    return 0.6f;
}

static float GetThrowArcHeight() {
    return 120.0f;
}

static float GetGroundAnimSeconds() {
    return 2.0f;
}

static float GetAshikScale() {
    return 0.1f; // 10x smaller; tweak as needed
}

static std::vector<Rectangle> DetectFrameRects(const char* spritePath) {
    Image image = LoadImage(spritePath);
    if (image.data == nullptr) {
        return {};
    }

    ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    Color* pixels = LoadImageColors(image);
    if (pixels == nullptr) {
        UnloadImage(image);
        return {};
    }

    const int w = image.width;
    const int h = image.height;
    std::vector<bool> colHas(w, false);
    for (int x = 0; x < w; ++x) {
        for (int y = 0; y < h; ++y) {
            if (pixels[y * w + x].a != 0) {
                colHas[x] = true;
                break;
            }
        }
    }

    struct Run { int start; int end; };
    std::vector<Run> runs;
    bool inRun = false;
    int start = 0;
    for (int x = 0; x < w; ++x) {
        if (colHas[x]) {
            if (!inRun) {
                inRun = true;
                start = x;
            }
        } else if (inRun) {
            inRun = false;
            runs.push_back({ start, x - 1 });
        }
    }
    if (inRun) {
        runs.push_back({ start, w - 1 });
    }

    const int minGap = 2;
    std::vector<Run> merged;
    for (const auto& run : runs) {
        if (!merged.empty() && run.start - merged.back().end - 1 <= minGap) {
            merged.back().end = run.end;
        } else {
            merged.push_back(run);
        }
    }

    std::vector<Rectangle> frames;
    if (!merged.empty()) {
        std::vector<int> starts(merged.size());
        std::vector<int> ends(merged.size());
        starts[0] = 0;
        for (size_t i = 1; i < merged.size(); ++i) {
            const int boundary = (merged[i - 1].end + merged[i].start) / 2;
            ends[i - 1] = boundary;
            starts[i] = boundary + 1;
        }
        ends.back() = w - 1;

        for (size_t i = 0; i < starts.size(); ++i) {
            const int width = ends[i] - starts[i] + 1;
            if (width <= 0) {
                continue;
            }
            frames.push_back(Rectangle{
                static_cast<float>(starts[i]),
                0.0f,
                static_cast<float>(width),
                static_cast<float>(h)
            });
        }
    }

    UnloadImageColors(pixels);
    UnloadImage(image);
    return frames;
}

struct AshikThrow {
    Texture2D texture{};
    SpriteAnimation animation{};
    bool active = false;
    bool inFlight = false;
    bool onGround = false;
    float flightTime = 0.0f;
    float groundTime = 0.0f;
    Vector2 start{};
    Vector2 end{};
    Vector2 pos{};

    void Load(const char* path, int frames = 5, float fps = 12.0f) {
        if (!FileExists(path)) {
            return;
        }
        texture = LoadTexture(path);
        std::vector<Rectangle> frameRects = DetectFrameRects(path);
        if (frameRects.size() >= 2) {
            animation.Init(texture, frameRects, fps, true);
        } else {
            animation.Init(texture, frames, fps, true);
        }
    }

    void Start(Vector2 startPos, float groundY, float distance) {
        if (texture.id == 0) {
            return;
        }
        const float scale = GetAshikScale();
        active = true;
        inFlight = true;
        onGround = false;
        flightTime = 0.0f;
        groundTime = 0.0f;
        start = startPos;
        pos = startPos;
        const float ashikHeight = (float)animation.GetFrameHeight() * scale;
        end = Vector2{ startPos.x + distance, groundY - ashikHeight };
        animation.Reset();
    }

    void Update(float dt) {
        if (!active) {
            return;
        }

        if (inFlight) {
            flightTime += dt;
            const float duration = GetThrowFlightSeconds();
            float t = (duration <= 0.0f) ? 1.0f : (flightTime / duration);
            if (t >= 1.0f) {
                t = 1.0f;
                inFlight = false;
                onGround = true;
                pos = end;
                animation.Reset();
                return;
            }

            const float arc = GetThrowArcHeight() * std::sinf(t * 3.14159265f);
            pos.x = start.x + (end.x - start.x) * t;
            pos.y = start.y + (end.y - start.y) * t - arc;
            return;
        }

        if (onGround) {
            if (groundTime < GetGroundAnimSeconds()) {
                groundTime += dt;
                animation.Update(dt);
            }
        }
    }

    void Draw() const {
        if (!active) {
            return;
        }
        animation.DrawScaled(pos, GetAshikScale());
    }

    void Unload() {
        if (texture.id != 0) {
            UnloadTexture(texture);
            texture = Texture2D{};
        }
    }
};

int main() {
    const int initialW = 1280;
    const int initialH = 720;

    InitWindow(initialW, initialH, "Ashyk - Raylib");
    SetTargetFPS(60);
    ChangeDirectory(GetApplicationDirectory());

    Texture2D background = {};
    const char* backgroundPath = "assets/background/background.png";
    if (FileExists(backgroundPath)) {
        background = LoadTexture(backgroundPath);
    } else if (FileExists("assets/background/background.jpg")) {
        background = LoadTexture("assets/background/background.jpg");
    } else {
        TraceLog(LOG_ERROR, "Missing background: %s", backgroundPath);
    }

    // --- GAME SETUP ---
    std::vector<std::string> names = {"Andy", "Mehmet", "Anna"};
    Game::setMaxRounds(10);
    Game game(names);

    std::string status = "Press SPACE to play a round";

    // --- SCORE STRATEGY ---
    std::unique_ptr<ScoreStrategy> strategy = std::make_unique<ClassicScoreStrategy>();

    // --- PLAYER ANIMATION ---
    PlayerAnimator playerAnimator;
    const char* spritePath = "assets/player/throw_strip.png";
    if (!FileExists(spritePath)) {
        TraceLog(LOG_ERROR, "Missing sprite: %s", spritePath);
    }
    playerAnimator.Load(spritePath, 5, 12.0f);
    playerAnimator.SetPosition(Vector2{ 40.0f, initialH * 0.7f - 180.0f });
    playerAnimator.SetReleaseFrame(4);

    // --- ASHIK THROW ---
    AshikThrow ashik;
    const char* ashikPath = "assets/ashiks/ashik.png";
    if (!FileExists(ashikPath)) {
        TraceLog(LOG_ERROR, "Missing ashik: %s", ashikPath);
    }
    ashik.Load(ashikPath, 5, 16.0f);

    while (!WindowShouldClose()) {
        const float dt = GetFrameTime();
        const int w = GetScreenWidth();
        const int h = GetScreenHeight();
        const float groundY = playerAnimator.GetPosition().y + (float)playerAnimator.GetFrameHeight();

        // INPUT
        if (IsKeyPressed(KEY_SPACE)) {
            try {
                game.playRound();
                status = "Round played!";
                playerAnimator.StartThrow();
            } catch (const GameError& e) {
                status = e.what();
            } catch (const std::exception& e) {
                status = std::string("Unexpected error: ") + e.what();
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

        if (playerAnimator.Update(dt)) {
            Vector2 release = playerAnimator.GetReleaseWorldPosition();
            ashik.Start(release, groundY, GetThrowDistance());
        }
        ashik.Update(dt);

        // DRAW
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (background.id != 0) {
            Rectangle src = { 0.0f, 0.0f, (float)background.width, (float)background.height };
            Rectangle dst = { 0.0f, 0.0f, (float)w, (float)h };
            DrawTexturePro(background, src, dst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        }

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

        DrawLine(0, (int)groundY, w, (int)groundY, DARKGRAY);

        ashik.Draw();
        playerAnimator.Draw();

        EndDrawing();
    }

    playerAnimator.Unload();
    ashik.Unload();
    if (background.id != 0) {
        UnloadTexture(background);
    }
    CloseWindow();
    return 0;
}
