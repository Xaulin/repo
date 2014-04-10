#pragma once
#include <cstdio>
#include <string.h>
#include <exception>
#include "GL\glew.h"
#include "GL\glm\glm.hpp"

class Model
{
public:
	friend Model* loadModel(char* filepath);
	friend Model* loadHeightMap(float x, float y, float z, char* path);
	~Model();
	GLuint* VBOs;
	unsigned vcount;
};