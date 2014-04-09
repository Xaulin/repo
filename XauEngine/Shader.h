#pragma once
#include "Model.h"
#include "Texture.h"

enum ObjectOptions{
	None = 0,
	NoDepthTest = 1,
	NoShadow = 2,
	NoCullFacing = 4,
	CullFacing = 8,
	NoLight = 16,
	SimpleShader = 32,
	StickToCamera = 64,
	SkyBox = 128 + 64 + 32 + 16 + 2 + 1,
	ShadowToTexture = 256,
};

struct Object{
	Model* model;
	Texture* texture;
};

struct ObjectsArrayElement{
	Object object;
	glm::mat4 mat;
	ObjectOptions options;
};

class Shader
{
public:
	virtual void draw(ObjectsArrayElement** objArray, unsigned count) = 0;
};

