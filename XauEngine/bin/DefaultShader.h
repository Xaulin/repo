#pragma once
#include "Shader.h"
#include "GL\glew.h"
#include "GL\glm\gtx\transform.hpp"
#include <vector>
#define MAX_LIGHTS 5
#define FBO_W 2048
#define FBO_H 2048
#define FBO_T GL_DEPTH_COMPONENT24

class DefaultShader : public Shader{
public:
	virtual void draw(ObjectsArrayElement** objArray, unsigned count);

	DefaultShader();
	DefaultShader(glm::ivec4& viewport, glm::vec3& eye, glm::vec3& pos);
	~DefaultShader();
	
	void setViewport(glm::ivec4& vec);
	void setCamera(glm::vec3& eye, glm::vec3& pos);
	void addLight(glm::vec3& vec);
	void setLight(unsigned slot, glm::vec3& vec);

private:
	GLuint program;
	GLuint shadowProgram;
	GLuint simpleProgram;

	//Camera options
	glm::vec3 eye, pos;
	glm::ivec4 viewport;

	//Light
	struct LightData{
		GLuint fb;
		GLuint dtext;
		glm::mat4 mat;
		glm::vec3 dir;
	};
	std::vector<LightData> lights;
};

