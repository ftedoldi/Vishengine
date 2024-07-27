#pragma once

struct Scale {
    Scale() = default;
    explicit Scale(const float scale) : Value{scale} {}

    float Value{1.f};
};