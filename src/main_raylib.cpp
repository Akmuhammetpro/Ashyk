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

static float GetThrowMinDistance() {
    return 60.0f;
}

static float GetThrowMaxDistance() {
    return 1000.0f;
}

static float GetThrowFlightSeconds() {
    return 0.6f;
}

static float GetDefaultThrowPower() {
    return 0.5f;
}

static float GetPowerDistanceScale(float power) {
    if (power < 0.0f) power = 0.0f;
    if (power > 1.0f) power = 1.0f;
    return 0.25f + 0.75f * power; // low power falls short
}

static float GetThrowArcHeight() {
    return 120.0f;
}

static float GetGroundAnimSeconds() {
    return 2.0f;
}

static float RandomFloat(float minValue, float maxValue) {
    const int r = GetRandomValue(0, 1000);
    return minValue + (maxValue - minValue) * (r / 1000.0f);
}

static float GetAshikScale() {
    return 0.2f; // 2x bigger than before; tweak as needed
}

static float GetSilverAshikScale() {
    return 0.2f;
}

static float GetSilverThrowSpeed() {
    return 520.0f;
}

static float GetSilverUpBoost() {
    return 140.0f;
}

static float GetGravity() {
    return 1200.0f;
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
    const int minPixels = (h / 50 < 6) ? 6 : (h / 50);
    const int minRunWidth = (w / 100 < 12) ? 12 : (w / 100);

    std::vector<bool> colHas(w, false);
    for (int x = 0; x < w; ++x) {
        int count = 0;
        for (int y = 0; y < h; ++y) {
            if (pixels[y * w + x].a != 0) {
                count++;
            }
        }
        if (count >= minPixels) {
            colHas[x] = true;
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

    const int minGap = 4;
    std::vector<Run> merged;
    for (const auto& run : runs) {
        if (!merged.empty() && run.start - merged.back().end - 1 <= minGap) {
            merged.back().end = run.end;
        } else {
            merged.push_back(run);
        }
    }

    std::vector<Rectangle> frames;
    for (const auto& run : merged) {
        const int width = run.end - run.start + 1;
        if (width < minRunWidth) {
            continue;
        }
        frames.push_back(Rectangle{
            static_cast<float>(run.start),
            0.0f,
            static_cast<float>(width),
            static_cast<float>(h)
        });
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
    float flightDuration = 0.0f;
    float groundTime = 0.0f;
    float power = 0.5f;
    Vector2 start{};
    Vector2 end{};
    Vector2 pos{};
    Vector2 direction{ 1.0f, 0.0f };

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

    void Start(Vector2 startPos, float groundY, float distance, float powerNorm) {
        if (texture.id == 0) {
            return;
        }
        const float scale = GetAshikScale();
        active = true;
        inFlight = true;
        onGround = false;
        flightTime = 0.0f;
        groundTime = 0.0f;
        if (powerNorm < 0.0f) powerNorm = 0.0f;
        if (powerNorm > 1.0f) powerNorm = 1.0f;
        power = powerNorm;
        const float speedFactor = 0.6f + 1.0f * powerNorm;
        flightDuration = GetThrowFlightSeconds() / speedFactor;
        const float distanceScale = GetPowerDistanceScale(powerNorm);
        start = startPos;
        pos = startPos;
        const float ashikHeight = (float)animation.GetFrameHeight() * scale;
        end = Vector2{ startPos.x + distance * distanceScale, groundY - ashikHeight };
        Vector2 delta = Vector2{ end.x - start.x, end.y - start.y };
        const float len = std::sqrt(delta.x * delta.x + delta.y * delta.y);
        if (len > 0.001f) {
            direction = Vector2{ delta.x / len, delta.y / len };
        } else {
            direction = Vector2{ 1.0f, 0.0f };
        }
        animation.Reset();
    }

    void Update(float dt) {
        if (!active) {
            return;
        }

        if (inFlight) {
            flightTime += dt;
            const float duration = (flightDuration <= 0.0f) ? 0.01f : flightDuration;
            float t = (duration <= 0.0f) ? 1.0f : (flightTime / duration);
            if (t >= 1.0f) {
                inFlight = false;
                onGround = true;
                pos = end;
                animation.Reset();
                return;
            }

            const float arc = GetThrowArcHeight() * std::sin(t * 3.14159265f);
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

    Rectangle GetBounds() const {
        const float scale = GetAshikScale();
        return Rectangle{
            pos.x,
            pos.y,
            (float)animation.GetFrameWidth() * scale,
            (float)animation.GetFrameHeight() * scale
        };
    }

    Vector2 GetDirection() const {
        return direction;
    }

    float GetPower() const {
        return power;
    }

    void Unload() {
        if (texture.id != 0) {
            UnloadTexture(texture);
            texture = Texture2D{};
        }
    }
};

enum class SilverState { Rest, Flight, GroundAnim, GroundStill };

struct SilverAshik {
    Texture2D texture{};
    SpriteAnimation animation{};
    SilverState state = SilverState::Rest;
    Vector2 pos{};
    Vector2 vel{};
    float groundTime = 0.0f;
    float scale = 1.0f;

    void Init(const Texture2D& tex, const std::vector<Rectangle>& frames, float fps, float inScale) {
        texture = tex;
        scale = inScale;
        if (frames.size() >= 2) {
            animation.Init(texture, frames, fps, true);
        } else {
            animation.Init(texture, 5, fps, true);
        }
        animation.Reset();
    }

    float Width() const { return (float)animation.GetFrameWidth() * scale; }
    float Height() const { return (float)animation.GetFrameHeight() * scale; }

    void Place(Vector2 groundPos) {
        pos = Vector2{ groundPos.x, groundPos.y - Height() };
        vel = Vector2{ 0.0f, 0.0f };
        groundTime = 0.0f;
        state = SilverState::Rest;
        animation.Reset();
    }

    void StartFlight(Vector2 velocity) {
        vel = velocity;
        state = SilverState::Flight;
    }

    bool IsIdleTarget() const {
        return state == SilverState::Rest || state == SilverState::GroundStill;
    }

    Rectangle GetBounds() const {
        return Rectangle{ pos.x, pos.y, Width(), Height() };
    }

    void Update(float dt, float groundY) {
        if (state == SilverState::Flight) {
            vel.y += GetGravity() * dt;
            pos.x += vel.x * dt;
            pos.y += vel.y * dt;

            if (pos.y + Height() >= groundY) {
                pos.y = groundY - Height();
                state = SilverState::GroundAnim;
                groundTime = 0.0f;
                animation.Reset();
            }
            return;
        }

        if (state == SilverState::GroundAnim) {
            if (groundTime < GetGroundAnimSeconds()) {
                groundTime += dt;
                animation.Update(dt);
            } else {
                state = SilverState::GroundStill;
            }
        }
    }

    void Draw() const {
        if (texture.id == 0) {
            return;
        }
        animation.DrawScaled(pos, scale);
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

    Texture2D silverTexture = {};
    std::vector<Rectangle> silverFrames;
    const char* silverPath = "assets/ashiks/silver_ashik.png";
    if (FileExists(silverPath)) {
        silverTexture = LoadTexture(silverPath);
        silverFrames = DetectFrameRects(silverPath);
    } else {
        TraceLog(LOG_ERROR, "Missing silver ashik: %s", silverPath);
    }

    std::vector<SilverAshik> silverAshiks;
    const int silverCount = 4;
    if (silverTexture.id != 0) {
        silverAshiks.resize(silverCount);
        for (int i = 0; i < silverCount; ++i) {
            silverAshiks[i].Init(silverTexture, silverFrames, 12.0f, GetSilverAshikScale());
        }
        const float tempGroundY = playerAnimator.GetPosition().y + (float)playerAnimator.GetFrameHeight();
        const float spacing = 18.0f;
        const float itemW = silverAshiks[0].Width();
        const float totalW = silverCount * itemW + (silverCount - 1) * spacing;
        float startX = (initialW - totalW) * 0.5f;
        for (int i = 0; i < silverCount; ++i) {
            silverAshiks[i].Place(Vector2{ startX + i * (itemW + spacing), tempGroundY });
        }
    }

    float pendingThrowDistance = GetThrowDistance();
    float pendingThrowPower = GetDefaultThrowPower();
    bool throwQueued = false;
    bool isCharging = false;
    float powerValue = 0.0f;
    float powerDir = 1.0f;
    Vector2 aimOrigin{};
    float aimDistance = GetThrowDistance();
    const int mePlayerIndex = 0;
    int activePlayerIndex = 0;
    bool turnLocked = false;
    float npcDelay = 0.0f;
    float npcDelayTarget = RandomFloat(0.25f, 0.8f);
    const int playerCount = (int)game.getPlayers().size();

    while (!WindowShouldClose()) {
        const float dt = GetFrameTime();
        const int w = GetScreenWidth();
        const int h = GetScreenHeight();
        const float groundY = playerAnimator.GetPosition().y + (float)playerAnimator.GetFrameHeight();
        const Vector2 mouse = GetMousePosition();
        const bool isPlayerTurn = (activePlayerIndex == mePlayerIndex);
        const bool isThrowBusy = ashik.inFlight || playerAnimator.IsThrowing();
        aimOrigin = playerAnimator.GetReleaseWorldPosition();
        const float minX = aimOrigin.x + GetThrowMinDistance();
        const float maxX = aimOrigin.x + GetThrowMaxDistance();
        float targetX = mouse.x;
        if (targetX < minX) targetX = minX;
        if (targetX > maxX) targetX = maxX;
        if (isPlayerTurn) {
            aimDistance = targetX - aimOrigin.x;
        }

        auto QueueThrow = [&](float distance, float power) {
            try {
                game.playRound();
                status = "Round played!";
                playerAnimator.StartThrow();
                pendingThrowDistance = distance;
                pendingThrowPower = power;
                throwQueued = true;
                turnLocked = true;
            } catch (const GameError& e) {
                status = e.what();
            } catch (const std::exception& e) {
                status = std::string("Unexpected error: ") + e.what();
            }
        };

        // INPUT
        if (isPlayerTurn && IsKeyPressed(KEY_SPACE) && !isThrowBusy && !isCharging) {
            QueueThrow(GetThrowDistance(), GetDefaultThrowPower());
        }

        if (isPlayerTurn && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (!isThrowBusy) {
                isCharging = true;
                powerValue = 0.0f;
                powerDir = 1.0f;
            }
        }
        if (isCharging) {
            powerValue += dt * 100.0f * powerDir;
            if (powerValue >= 100.0f) {
                powerValue = 100.0f;
                powerDir = -1.0f;
            } else if (powerValue <= 0.0f) {
                powerValue = 0.0f;
                powerDir = 1.0f;
            }
        }
        if (isCharging && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            isCharging = false;
            QueueThrow(aimDistance, powerValue / 100.0f);
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
            const float distance = throwQueued ? pendingThrowDistance : GetThrowDistance();
            const float power = throwQueued ? pendingThrowPower : GetDefaultThrowPower();
            ashik.Start(release, groundY, distance, power);
            throwQueued = false;
        }
        ashik.Update(dt);

        if (turnLocked && !ashik.inFlight && !playerAnimator.IsThrowing()) {
            turnLocked = false;
            activePlayerIndex = (playerCount > 0) ? ((activePlayerIndex + 1) % playerCount) : 0;
            if (activePlayerIndex != mePlayerIndex) {
                npcDelay = 0.0f;
                npcDelayTarget = RandomFloat(0.25f, 0.8f);
            }
        }

        if (!isPlayerTurn && !turnLocked && !isThrowBusy && !isCharging) {
            npcDelay += dt;
            if (npcDelay >= npcDelayTarget) {
                const float randomDistance = RandomFloat(GetThrowMinDistance(), GetThrowMaxDistance());
                const float randomPower = RandomFloat(0.2f, 1.0f);
                QueueThrow(randomDistance, randomPower);
                npcDelay = 0.0f;
                npcDelayTarget = RandomFloat(0.25f, 0.8f);
            }
        }

        if (ashik.inFlight) {
            Rectangle a = ashik.GetBounds();
            const Vector2 dir = ashik.GetDirection();
            const float speed = GetSilverThrowSpeed() * (0.6f + 0.9f * ashik.GetPower());
            const Vector2 hitVelocity = Vector2{
                dir.x * speed,
                -std::fabs(dir.y) * speed - GetSilverUpBoost()
            };
            for (auto& s : silverAshiks) {
                if (!s.IsIdleTarget()) {
                    continue;
                }
                if (CheckCollisionRecs(a, s.GetBounds())) {
                    s.StartFlight(hitVelocity);
                }
            }
        }

        for (auto& s : silverAshiks) {
            s.Update(dt, groundY);
        }

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

        DrawText("Move mouse - aim, hold LMB - power, release to throw", 40, h - 90, 18, DARKGRAY);
        DrawText("SPACE - quick throw, 1/2/3 - change strategy", 40, h - 65, 18, DARKGRAY);
        DrawText("F - fullscreen ON/OFF, ESC - exit", 40, h - 40, 18, DARKGRAY);

        DrawLine(0, (int)groundY, w, (int)groundY, DARKGRAY);

        if (isPlayerTurn) {
            DrawLine((int)targetX, (int)groundY - 35, (int)targetX, (int)groundY + 5, ORANGE);
            DrawCircleLines((int)targetX, (int)groundY - 18, 10.0f, ORANGE);
        }

        if (isCharging) {
            const int barW = 220;
            const int barH = 14;
            const int barX = (w - barW) / 2;
            const int barY = 10;
            DrawRectangleLines(barX, barY, barW, barH, BLACK);
            int fillW = (int)(barW * (powerValue / 100.0f));
            if (fillW < 0) fillW = 0;
            if (fillW > barW) fillW = barW;
            const int innerW = (fillW > 2) ? (fillW - 2) : 0;
            if (innerW > 0) {
                DrawRectangle(barX + 1, barY + 1, innerW, barH - 2, ORANGE);
            }
            DrawText(TextFormat("Power: %d", (int)powerValue), barX, barY + barH + 6, 18, BLACK);
        }

        for (const auto& s : silverAshiks) {
            s.Draw();
        }
        const char* label = isPlayerTurn ? "Me" : game.getPlayers()[activePlayerIndex].getName().c_str();
        const int labelSize = 18;
        const int labelWidth = MeasureText(label, labelSize);
        const float labelX = playerAnimator.GetPosition().x + (float)playerAnimator.GetFrameWidth() * 0.5f - labelWidth * 0.5f;
        const float labelY = playerAnimator.GetPosition().y - 26.0f;
        DrawText(label, (int)labelX, (int)labelY, labelSize, BLACK);
        ashik.Draw();
        playerAnimator.Draw();

        EndDrawing();
    }

    playerAnimator.Unload();
    ashik.Unload();
    if (silverTexture.id != 0) {
        UnloadTexture(silverTexture);
    }
    if (background.id != 0) {
        UnloadTexture(background);
    }
    CloseWindow();
    return 0;
}
