#pragma once
#include "Texture.h"
#include "Model.h"
#include "GL\glm\gtx\transform.hpp"

class Object{
public:
	friend class Scene;

	Object();
	Object(Model* model, Texture* texture);
	~Object();

	void setModel(Model* model);
	void setTexture(Texture* texture);

	void move(glm::vec3& vec);
	void rotate(float& rangle, glm::vec3& vec);
	void scale(glm::vec3& vec);
	void setSpeed(float& perSec);

private:
	void update(unsigned time);
	void setTime(unsigned time);

	Model* model;
	Texture* texture;
	glm::mat4 mat;

	glm::vec3 mvec, rvec, svec,
		mvec2, svec2;
	float rangle, rangle2;
	float speed;

	unsigned lastTime;
};

