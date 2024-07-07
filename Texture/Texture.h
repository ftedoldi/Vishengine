#pragma once

#include "glad/gl.h"

#include <string>
#include <iostream>

class Texture {
public:
	void createTexture(const std::string& path);
	void bindTexture(unsigned int texNum) const;

private:
	unsigned int _id = 0;
};