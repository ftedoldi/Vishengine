#pragma once

#include "glad/gl.h"

#include <string>
#include <iostream>

class Texture {
public:
	void CreateTexture(const std::string& path);
	void BindTexture(unsigned texNum) const;

private:
	unsigned _id = 0;
};