#pragma once

#include "Texture.h"

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

#include <unordered_map>

struct MaterialData {
    std::vector<Texture> TexturesDiffuse{};
    std::vector<Texture> TexturesSpecular{};
    std::vector<Texture> TexturesNormal{};
    glm::vec4 ColorDiffuse{};
    glm::vec3 ColorSpecular{};
};

class MaterialController {
public:
    void AddMaterial(uint32_t meshID, MaterialData&& materialData);

    const MaterialData& GetMaterialData(uint32_t meshID) const;

    void UpdateMaterialData(uint32_t meshID, MaterialData&& materialData);

private:
    std::unordered_map<uint32_t, MaterialData> _materialIDToTexturesList{};
};
