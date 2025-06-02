#pragma once

#include "Components/Material.h"
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
    Material AddMaterial(MaterialData&& materialData);

    const MaterialData& GetMaterialData(const MaterialID& materialID) const;

    void UpdateMaterialData(const MaterialID& materialID, MaterialData&& materialData);

private:
    std::unordered_map<MaterialID, MaterialData> _materialIDToTexturesList{};
};
