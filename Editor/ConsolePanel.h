#pragma once

#include "IPanel.h"

#include <string>
#include <vector>

enum class LogLevel {
    Info,
    Warning,
    Error
};

struct LogEntry {
    LogLevel    level{LogLevel::Info};
    std::string message;
};

/**
 * @brief Displays engine log messages (bottom-left panel).
 *
 * Other systems can push messages via the static Log() helpers.
 * The panel renders them with color-coded severity.
 */
class ConsolePanel final : public IPanel {
public:
    ConsolePanel() = default;

    void OnRender(entt::registry& registry) override;

    /** Push a new log entry. */
    void Log(LogLevel level, const std::string& message);
    void LogInfo   (const std::string& msg) { Log(LogLevel::Info,    msg); }
    void LogWarning(const std::string& msg) { Log(LogLevel::Warning, msg); }
    void LogError  (const std::string& msg) { Log(LogLevel::Error,   msg); }

    void Clear() { _entries.clear(); }

private:
    std::vector<LogEntry> _entries{};
    bool _scrollToBottom{false};

    bool _showInfo{true};
    bool _showWarnings{true};
    bool _showErrors{true};
};
