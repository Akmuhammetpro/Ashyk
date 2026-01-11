#pragma once
#include "raylib.h"
#include "SpriteAnimation.h"
#include <vector>

class PlayerAnimator {
public:
    enum class State { Idle, Throwing };

    void Load(const char* throwSpritePath, int frames = 6, float fps = 12.0f) {
        throwTexture_ = LoadTexture(throwSpritePath);
        std::vector<Rectangle> frameRects = DetectFrameRects(throwSpritePath);
        if (frameRects.size() >= 2) {
            throwAnimation_.Init(throwTexture_, frameRects, fps, false);
        } else {
            throwAnimation_.Init(throwTexture_, frames, fps, false);
        }

        state_ = State::Idle;
        hasReleased_ = false;
        releaseFrame_ = 2; // на 3-м кадре "реальный бросок"
    }

    void Unload() {
        if (throwTexture_.id != 0) {
            UnloadTexture(throwTexture_);
            throwTexture_ = Texture2D{};
        }
    }

    void SetPosition(Vector2 position) { position_ = position; }
    void SetReleaseFrame(int frameIndex) { releaseFrame_ = frameIndex; }
    Vector2 GetPosition() const { return position_; }
    int GetFrameHeight() const { return throwAnimation_.GetFrameHeight(); }
    int GetFrameWidth() const { return throwAnimation_.GetFrameWidth(); }

    void StartThrow() {
        state_ = State::Throwing;
        throwAnimation_.Reset();
        hasReleased_ = false;
    }

    // Возвращает true ровно один раз — когда наступил кадр выпуска
    bool Update(float deltaTime) {
        if (state_ != State::Throwing) return false;

        throwAnimation_.Update(deltaTime);

        bool shouldRelease = false;
        if (!hasReleased_ && throwAnimation_.GetFrame() >= releaseFrame_) {
            hasReleased_ = true;
            shouldRelease = true;
        }

        if (throwAnimation_.IsFinished()) {
            state_ = State::Idle;
        }

        return shouldRelease;
    }

    void Draw() const {
        throwAnimation_.Draw(position_);
    }

    bool IsThrowing() const { return state_ == State::Throwing; }

    Vector2 GetReleaseWorldPosition() const {
        Rectangle frame = throwAnimation_.GetSourceRect();
        return Vector2{
            position_.x + frame.width * 0.82f,
            position_.y + frame.height * 0.35f
        };
    }

private:
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

    Vector2 position_{ 300, 350 };

    Texture2D throwTexture_{};
    SpriteAnimation throwAnimation_{};

    State state_ = State::Idle;

    bool hasReleased_ = false;
    int releaseFrame_ = 2;
};
