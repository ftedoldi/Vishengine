#include "MaterialController.h"

#include <cassert>

void MaterialController::AddMaterial(const uint32_t meshID, MaterialData&& materialData) {
    _materialIDToTexturesList.emplace(meshID, std::move(materialData));
}

const MaterialData& MaterialController::GetMaterialData(const uint32_t meshID) const {
    assert(_materialIDToTexturesList.contains(meshID));

    return _materialIDToTexturesList.at(meshID);
}

void MaterialController::UpdateMaterialData(const uint32_t meshID, MaterialData&& materialData) {
    _materialIDToTexturesList.insert_or_assign(meshID, std::move(materialData));
}
