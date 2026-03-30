#pragma once

namespace WindowsEvents {

    struct FrameBufferSizeChangedEvent {
        float Width{};
        float Height{};
    };

    struct KeyPressedEvent {
        int Key{};
        int Scancode{};
        int Action{};
        int Mods{};
    };

    struct MouseMovedEvent {
        double XMousePosition{};
        double YMousePosition{};
    };

}