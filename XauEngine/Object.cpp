#include "Object.h"

Object::Object() :
model(0),
texture(0),
speed(1),
rangle(rangle2 = 0){}
Object::Object(Model* model, Texture* texture){
	this->model = model;
	this->texture = texture;
}
Object::~Object(){
	delete[] model;
	delete[] texture;
}

void Object::setModel(Model* model){
	this->model = model;
}
void Object::setTexture(Texture* texture){
	this->texture = texture;
}

void Object::move(glm::vec3& vec){
	mvec = mvec2 = vec;
}
void Object::rotate(float& rangle, glm::vec3& vec){
	this->rangle = this->rangle2 = rangle;
	rvec = vec;
}
void Object::scale(glm::vec3& vec){
	svec = svec2 = vec;
}
void Object::setSpeed(float& perSec){
	speed = perSec;
}

void Object::update(unsigned time){
	mat = glm::scale(svec)*
		glm::rotate(rangle, rvec)*
		glm::translate(mvec);
	lastTime = time;
}
void Object::setTime(unsigned time){

}