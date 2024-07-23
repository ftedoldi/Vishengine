#pragma once

#include "Texture/Texture.h"

#include <vector>

struct TextureList {
    std::vector<Texture> TexturesDiffuse{};
    std::vector<Texture> TexturesSpecular{};
    std::vector<Texture> TexturesNormal{};
};
