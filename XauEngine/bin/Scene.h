#pragma once
#include <assert.h>
#include "GL\glm\gtx\transform.hpp"
#include "Model.h"
#include "Texture.h"
#include "Shader.h"

class Scene{
public:
	Scene(unsigned objectElements);
	~Scene();

	void addObject(unsigned slot, Object obj);
	void addObject(unsigned slot, Object obj, int options);
	
	void delObject(unsigned slot);

	void move(unsigned slot, glm::vec3& vec);
	void scale(unsigned slot, glm::vec3& vec);
	void rotate(unsigned slot, glm::vec4& vec);

	void moveNext(unsigned slot, glm::vec3& vec);
	void scaleNext(unsigned slot, glm::vec3& vec);
	void rotateNext(unsigned slot, glm::vec4& vec);

	void bindShader(Shader* shader);
	void delShader();

	void draw();

private:
	ObjectsArrayElement**objects;
	unsigned objectElements;
	Shader* currentShader;
};

