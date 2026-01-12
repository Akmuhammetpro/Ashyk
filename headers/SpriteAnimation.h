#pragma once
#include "raylib.h"
#include <vector>

class SpriteAnimation {
public:
    SpriteAnimation() = default;

    void Init(const Texture2D& texture, int frameCount, float fps, bool loop = false) {
        texture_ = texture;
        frames_.clear();
        frameCount_ = (frameCount <= 0 ? 1 : frameCount);
        fps_ = (fps <= 0.0f ? 12.0f : fps);
        loop_ = loop;

        currentFrame_ = 0;
        accumulator_ = 0.0f;

        frameWidth_ = texture_.width / frameCount_;
        frameHeight_ = texture_.height;
    }

    void Init(const Texture2D& texture, const std::vector<Rectangle>& frames, float fps, bool loop = false) {
        texture_ = texture;
        frames_ = frames;
        frameCount_ = frames_.empty() ? 1 : static_cast<int>(frames_.size());
        fps_ = (fps <= 0.0f ? 12.0f : fps);
        loop_ = loop;

        currentFrame_ = 0;
        accumulator_ = 0.0f;

        if (!frames_.empty()) {
            frameWidth_ = static_cast<int>(frames_[0].width);
            frameHeight_ = static_cast<int>(frames_[0].height);
        } else {
            frameWidth_ = texture_.width;
            frameHeight_ = texture_.height;
        }
    }

    void Reset() {
        currentFrame_ = 0;
        accumulator_ = 0.0f;
    }

    void Update(float deltaTime) {
        if (IsFinished()) return;

        accumulator_ += deltaTime;
        const float secondsPerFrame = 1.0f / fps_;

        while (accumulator_ >= secondsPerFrame) {
            accumulator_ -= secondsPerFrame;
            currentFrame_++;

            if (loop_ && currentFrame_ >= frameCount_) {
                currentFrame_ = 0;
            }
        }
    }

    bool IsFinished() const {
        return (!loop_ && currentFrame_ >= frameCount_);
    }

    int GetFrame() const {
        return currentFrame_;
    }

    int GetFrameWidth() const {
        return frameWidth_;
    }

    int GetFrameHeight() const {
        return frameHeight_;
    }

    Rectangle GetSourceRect() const {
        int idx = currentFrame_;
        if (idx < 0) idx = 0;
        if (idx >= frameCount_) idx = frameCount_ - 1;

        if (!frames_.empty()) {
            return frames_[idx];
        }

        return Rectangle{
            (float)(idx * frameWidth_),
            0.0f,
            (float)frameWidth_,
            (float)frameHeight_
        };
    }

    void Draw(Vector2 position) const {
        Rectangle src = GetSourceRect();
        Rectangle dst = { position.x, position.y, (float)src.width, (float)src.height };
        DrawTexturePro(texture_, src, dst, Vector2{0,0}, 0.0f, WHITE);
    }

    void DrawScaled(Vector2 position, float scale) const {
        Rectangle src = GetSourceRect();
        Rectangle dst = { position.x, position.y, src.width * scale, src.height * scale };
        DrawTexturePro(texture_, src, dst, Vector2{0,0}, 0.0f, WHITE);
    }

private:
    Texture2D texture_{};
    std::vector<Rectangle> frames_{};

    int frameCount_ = 1;
    int currentFrame_ = 0;

    float fps_ = 12.0f;
    float accumulator_ = 0.0f;

    int frameWidth_ = 0;
    int frameHeight_ = 0;

    bool loop_ = false;
};
