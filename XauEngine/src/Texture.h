#pragma once
#include <cstdio>
#include <string.h>
#include <exception>
#include "GL\glew.h"
#include "GL\glm\glm.hpp"

class Texture
{
public:
	friend Texture* loadTexture(char* filepath);
	~Texture();
	GLuint id;
};

