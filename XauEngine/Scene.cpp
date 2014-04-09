#include "Scene.h"

Scene::Scene(unsigned objectElements = 10) :
objects(new ObjectsArrayElement*[objectElements]),
objectElements(objectElements),
currentShader(0){
	for (unsigned i = 0; i < objectElements; ++i)
		objects[i] = 0;
}
Scene::~Scene(){
	for (unsigned i = 0; i < objectElements; ++i)
		delete objects[i];
	delete[] objects;
	delShader();
}

void Scene::addObject(unsigned slot, Object obj){
	objects[slot] = new ObjectsArrayElement;
	objects[slot]->object = obj;
	objects[slot]->options = None;
}
void Scene::addObject(unsigned slot, Object obj, int options){
	objects[slot] = new ObjectsArrayElement;
	objects[slot]->object = obj;
	objects[slot]->options = (ObjectOptions)options;
}
void Scene::delObject(unsigned slot){
	delete objects[slot]->object.model;
	delete objects[slot]->object.texture;
	delete objects[slot];
	objects[slot] = 0;
}

void Scene::move(unsigned slot, glm::vec3& vec){
#ifndef NDEBUG
	assert(objects[slot]);
#endif //NDEBUG
	objects[slot]->mat = glm::translate(vec);
}
void Scene::scale(unsigned slot, glm::vec3& vec){
#ifndef NDEBUG
	assert(objects[slot]);
#endif //NDEBUG
	objects[slot]->mat = glm::scale(vec);
}
void Scene::rotate(unsigned slot, glm::vec4& vec){
#ifndef NDEBUG
	assert(objects[slot]);
#endif //NDEBUG
	objects[slot]->mat = glm::rotate(vec.w,(glm::vec3)vec);
}

void Scene::moveNext(unsigned slot, glm::vec3& vec){
#ifndef NDEBUG
	assert(objects[slot]);
#endif //NDEBUG
	objects[slot]->mat *= glm::translate(vec);
}
void Scene::scaleNext(unsigned slot, glm::vec3& vec){
#ifndef NDEBUG
	assert(objects[slot]);
#endif //NDEBUG
	objects[slot]->mat *= glm::scale(vec);
}
void Scene::rotateNext(unsigned slot, glm::vec4& vec){
#ifndef NDEBUG
	assert(objects[slot]);
#endif //NDEBUG
	objects[slot]->mat *= glm::rotate(vec.w, (glm::vec3)vec);
}

void Scene::bindShader(Shader* shader){
	currentShader = shader;
}
void Scene::delShader(){
	delete currentShader;
	currentShader = 0;
}

void Scene::draw(){
	currentShader->draw(objects, objectElements);
}