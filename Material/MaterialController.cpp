#include "MaterialController.h"

#include <cassert>

Material MaterialController::AddMaterial(MaterialData&& materialData) {
    // TODO: Create the new material ID (with boost).
    static MaterialID materialID{0};
    _materialIDToTexturesList.emplace(materialID, std::move(materialData));
    return Material{materialID++};
}

const MaterialData& MaterialController::GetMaterialData(const MaterialID &materialID) const {
    assert(_materialIDToTexturesList.contains(materialID));

    return _materialIDToTexturesList.at(materialID);
}

void MaterialController::UpdateMaterialData(const MaterialID& materialID, MaterialData&& materialData) {
    _materialIDToTexturesList.insert_or_assign(materialID, std::move(materialData));
}
