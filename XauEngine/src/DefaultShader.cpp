#include "DefaultShader.h"

extern const char* vs;
extern const char* fs;

extern const char* dvs;
extern const char* dfs;

extern const char* svs;
extern const char* sfs;


extern const char* vs2;
extern const char* fs2;

extern const char* dvs2;
extern const char* dfs2;


#include <Windows.h>

float a = 0;
void DefaultShader::draw(ObjectsArrayElement** objArray, unsigned count){

	/*char buf[32];
	_itoa_s(glGetUniformLocation(program, "camdir"), buf, 10);
	MessageBoxA(0, buf, 0, 0);*/

	/*===INDECIES OF SHADER VARIABLES===
	vertexPosition_modelspace	0
	vertexNormal_modelspace		1
	vertexUV					2

	M							5
	VP							6
	DepthBiasMVP[]				0-4
	lightDir					7-11
	myTextureSampler			12
	shadowMap					13-15*/

	/*
	M				1
	VP				2
	DepthMVP		0
	textureSampler	6
	shadowMap		5
	ldir			4
	camdir			3
	*/


	glm::mat4 projShadow = glm::ortho(-20.f, 20.f, -20.f, 20.f, -10.f, 50.f);

	//=========================Generate shadow map=========================
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glUseProgram(shadowProgram);

	for (unsigned j = 0; j < lights.size(); ++j){
		glBindFramebuffer(GL_FRAMEBUFFER, lights[j].fb);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, FBO_W, FBO_H);

		for (unsigned i = 0; i < count; ++i){
			if (objArray[i] && !(objArray[i]->options & ObjectOptions::NoShadow)){

				lights[j].mat = projShadow *
					glm::lookAt(pos + lights[j].dir,
					pos, glm::vec3(0, 1, 0));

				glUniformMatrix4fv(0, 1, 0, &(lights[j].mat*objArray[i]->mat)[0][0]);
				glBindBuffer(GL_ARRAY_BUFFER, objArray[i]->object.model->VBOs[1]);
				glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
				glEnableVertexAttribArray(0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objArray[i]->object.model->VBOs[0]);
				glDrawElements(GL_TRIANGLES, objArray[i]->object.model->vcount, GL_UNSIGNED_INT, 0);
				glDisableVertexAttribArray(0);
			}
		}
	}

	//=========================Draw true scene=========================
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(viewport.x, viewport.y, viewport.z, viewport.w);

	glm::mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
		);

	glm::mat4 projView =
		glm::perspective(40.f, (float)viewport.z / (float)viewport.w, 0.001f, 1000.f)*
		glm::lookAt(eye, pos, glm::vec3(0, 1, 0));

	for (unsigned i = 0; i < count; ++i){
		if (objArray[i]){

			if (objArray[i]->options & ObjectOptions::NoDepthTest)
				glDisable(GL_DEPTH_TEST);
			else
				glEnable(GL_DEPTH_TEST);

			if (objArray[i]->options & ObjectOptions::NoCullFacing)
				glDisable(GL_CULL_FACE);
			else
				glEnable(GL_CULL_FACE);

			if (objArray[i]->options & ObjectOptions::CullFacing)
				glCullFace(GL_FRONT);
			else
				glCullFace(GL_BACK);

			//Simple Shader
			if (objArray[i]->options & ObjectOptions::SimpleShader){
				glUseProgram(simpleProgram);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, objArray[i]->object.texture->id);
				glUniform1i(1, 0);

				glm::mat4 mat = projView;
				if (objArray[i]->options & ObjectOptions::StickToCamera)
					mat *= glm::translate(eye);
				else
					mat *= objArray[i]->mat;
				glUniformMatrix4fv(0, 1, 0, &(mat[0][0]));

				glBindBuffer(GL_ARRAY_BUFFER, objArray[i]->object.model->VBOs[1]);
				glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
				glBindBuffer(GL_ARRAY_BUFFER, objArray[i]->object.model->VBOs[3]);
				glVertexAttribPointer(1, 2, GL_FLOAT, 0, 0, 0);

				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(1);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objArray[i]->object.model->VBOs[0]);
				glDrawElements(GL_TRIANGLES, objArray[i]->object.model->vcount, GL_UNSIGNED_INT, 0);
				glDisableVertexAttribArray(1);
				glDisableVertexAttribArray(0);
			}
			else{
				glUseProgram(program);

				//setup texture
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, objArray[i]->object.texture->id);
				glUniform1i(6, 0);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, lights[0].dtext);
				glUniform1i(5, 1);

				//setup matrix
				glUniformMatrix4fv(2, 1, 0, &(projView[0][0]));
				glUniformMatrix4fv(1, 1, 0, &(objArray[i]->mat[0][0]));
				glUniformMatrix4fv(0, 1, 0, &((biasMatrix *
					lights[0].mat * objArray[i]->mat)[0][0]));

				//setup light dir
				glUniform3fv(4, 1, &(glm::vec4(lights[0].dir, 1) * objArray[i]->mat)[0]);

				//setup cam pos
				glUniform3fv(3, 1, &pos[0]);

				//draw
				glBindBuffer(GL_ARRAY_BUFFER, objArray[i]->object.model->VBOs[1]);
				glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
				glBindBuffer(GL_ARRAY_BUFFER, objArray[i]->object.model->VBOs[2]);
				glVertexAttribPointer(1, 3, GL_FLOAT, 0, 0, 0);
				glBindBuffer(GL_ARRAY_BUFFER, objArray[i]->object.model->VBOs[3]);
				glVertexAttribPointer(2, 2, GL_FLOAT, 0, 0, 0);

				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(1);
				glEnableVertexAttribArray(2);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objArray[i]->object.model->VBOs[0]);
				glDrawElements(GL_TRIANGLES, objArray[i]->object.model->vcount, GL_UNSIGNED_INT, 0);

				glDisableVertexAttribArray(2);
				glDisableVertexAttribArray(1);
				glDisableVertexAttribArray(0);
			}
		}
	}
}

DefaultShader::DefaultShader(){

	//========================Compile shaders========================

	//===============Main shader===============

	GLuint vsid[6];
	vsid[0] = glCreateShader(GL_VERTEX_SHADER);
	vsid[1] = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vsid[0], 1, &vs2, 0);
	glCompileShader(vsid[0]);
	glShaderSource(vsid[1], 1, &fs2, 0);
	glCompileShader(vsid[1]);

	program = glCreateProgram();

	glAttachShader(program, vsid[0]);
	glAttachShader(program, vsid[1]);

	glLinkProgram(program);

#ifndef NDEBUG
	{
		GLsizei len;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
		char* log = new char[len];
		glGetProgramInfoLog(program, len, 0, log);

		if (log[0])
			throw std::exception(log, 6);

		delete[] log;
	}
#endif

	//===============Shadow shader===============

	vsid[2] = glCreateShader(GL_VERTEX_SHADER);
	vsid[3] = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vsid[2], 1, &dvs2, 0);
	glCompileShader(vsid[2]);
	glShaderSource(vsid[3], 1, &dfs2, 0);
	glCompileShader(vsid[3]);

	shadowProgram = glCreateProgram();

	glAttachShader(shadowProgram, vsid[2]);
	glAttachShader(shadowProgram, vsid[3]);

	glLinkProgram(shadowProgram);

#ifndef NDEBUG
	{
		GLsizei len;
		glGetProgramiv(shadowProgram, GL_INFO_LOG_LENGTH, &len);
		char* log = new char[len];
		glGetProgramInfoLog(shadowProgram, len, 0, log);

		if (log[0])
			throw std::exception(log, 6);

		delete[] log;
	}
#endif

	//===============Simple shader===============

	vsid[4] = glCreateShader(GL_VERTEX_SHADER);
	vsid[5] = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vsid[4], 1, &svs, 0);
	glCompileShader(vsid[4]);
	glShaderSource(vsid[5], 1, &sfs, 0);
	glCompileShader(vsid[5]);

	simpleProgram = glCreateProgram();

	glAttachShader(simpleProgram, vsid[4]);
	glAttachShader(simpleProgram, vsid[5]);

	glLinkProgram(simpleProgram);

#ifndef NDEBUG
	{
		GLsizei len;
		glGetProgramiv(simpleProgram, GL_INFO_LOG_LENGTH, &len);
		char* log = new char[len];
		glGetProgramInfoLog(simpleProgram, len, 0, log);

		if (log[0])
			throw std::exception(log, 6);

		delete[] log;
	}
#endif

	for each(GLuint a in vsid)
		glDeleteShader(a);
}
DefaultShader::DefaultShader(glm::ivec4& viewport, glm::vec3& eye, glm::vec3& pos){
	this->viewport = viewport;
	this->eye = eye;
	this->pos = pos;
	DefaultShader();
}
DefaultShader::~DefaultShader(){
	glDeleteProgram(program);
	glDeleteProgram(shadowProgram);
	glDeleteProgram(simpleProgram);
}

void DefaultShader::setViewport(glm::ivec4& viewport){
	this->viewport = viewport;
}
void DefaultShader::setCamera(glm::vec3& eye, glm::vec3& pos){
	this->eye = eye;
	this->pos = pos;
}
void DefaultShader::addLight(glm::vec3& vec){
	if (lights.size() >= MAX_LIGHTS)
		return;

	LightData ld;
	//========================Build Framebuffer========================
	glGenFramebuffers(1, &ld.fb);
	glBindFramebuffer(GL_FRAMEBUFFER, ld.fb);
	glGenTextures(1, &ld.dtext);
	glBindTexture(GL_TEXTURE_2D, ld.dtext);

	glTexImage2D(GL_TEXTURE_2D, 0, FBO_T, FBO_W, FBO_H, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ld.dtext, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

#ifndef NDEBUG
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::exception("Shaodw map framebuffer error", 9);
#endif

	ld.dir = glm::normalize(vec);
	lights.push_back(ld);
}
void DefaultShader::setLight(unsigned slot, glm::vec3& vec){
	lights[slot].dir = vec;
}