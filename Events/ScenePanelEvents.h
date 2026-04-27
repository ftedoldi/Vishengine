#pragma once

#include "glm/vec2.hpp"

namespace ScenePanelEvents {

struct ScenePanelResizedEvent {
    glm::vec2 ScenePanelSize{};
};

struct ScenePanelMouseMovedEvent {
    glm::vec2 ScenePanelMousePosition{};
};

}
