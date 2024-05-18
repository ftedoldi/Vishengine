#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

class Window
{
public:
	void InitializeWindow();
	bool ShouldWindowClose();
	void Update();

    static void Clear();

    /*
     * \brief Gets the width of the window.
     * \return Width of the window.
     */
    int GetWidth() const;

    /*
     * \brief Gets the height of the window.
     * \return height of the window.
     */
    int GetHeight() const;

private:
	GLFWwindow* _window = nullptr;
	std::string _windowName = {"Thot-engine"};
	static constexpr int _width = 800;
	static constexpr int _height = 600;

	void _setFramebufferSizeCallback();
};