#pragma once

#include "IPanel.h"

#include <filesystem>
#include <string>

/**
 * @brief Displays the project asset browser (bottom-centre panel).
 *
 * Navigates the Assets/ directory and shows folders / files as icon tiles,
 * similar to the Unity Project window.
 */
class ContentBrowserPanel final : public IPanel {
public:
    explicit ContentBrowserPanel(const std::filesystem::path& assetsRoot);

    void OnRender(entt::registry& registry) override;

private:
    void _renderBreadcrumb();
    void _renderDirectoryTree(const std::filesystem::path& dir);
    void _renderContents();

    std::filesystem::path _assetsRoot;
    std::filesystem::path _currentDir;

    /** Thumbnail size in pixels. */
    static constexpr float ThumbnailSize{72.f};
    static constexpr float Padding{16.f};
};
