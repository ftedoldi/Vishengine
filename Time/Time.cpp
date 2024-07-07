#include "Time.h"

#include <GLFW/glfw3.h>

float Time::_deltaTime{0.f};
float Time::_lastFrame{0.f};

float Time::GetDeltaTime(){
    return _deltaTime;
}

void Time::UpdateDeltaTime(){
    float currentTime{static_cast<float>(glfwGetTime())};
    _deltaTime = currentTime - _lastFrame;
    _lastFrame = currentTime;
}
