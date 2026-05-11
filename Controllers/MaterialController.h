#pragma once

#include "../Material/Texture.h"

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

#include <ankerl/unordered_dense.h>

#include <memory>
#include <vector>

struct MaterialData {
    std::vector<std::shared_ptr<Texture>> TexturesDiffuse{};
    std::vector<std::shared_ptr<Texture>> TexturesSpecular{};
    std::vector<std::shared_ptr<Texture>> TexturesNormal{};
    glm::vec4 ColorDiffuse{};
    glm::vec3 ColorSpecular{};
};

class MaterialController {
public:
    void AddMaterial(uint32_t meshID, MaterialData&& materialData);

    const MaterialData& GetMaterialData(uint32_t meshID) const;

    void UpdateMaterialData(uint32_t meshID, MaterialData&& materialData);

private:
    ankerl::unordered_dense::map<uint32_t, MaterialData> _materialIDToTexturesList{};
};
