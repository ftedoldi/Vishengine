#pragma once

class Time {
public:
    static void UpdateDeltaTime();

    static float GetDeltaTime();

private:
    static float _deltaTime;
    static float _lastFrame;
};
