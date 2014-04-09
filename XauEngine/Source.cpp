#include <Windows.h>
#include "Scene.h"
#include "DefaultShader.h"
#include "GL\glfw3.h"
#include <ctime>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "GL/glew32.lib")
#pragma comment(lib, "GL/glfw3.lib")

#define M_PI 3.14159265358979323846

GLFWwindow* window;
double cx = 0, cy = 0;
bool keysState[5];
float aa = 3.14f, b = 0;
glm::vec4 camPos(0, 8, 0, 1);
glm::vec4 camTarget(0, 0, 0, 1);

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	if (key == GLFW_KEY_W){
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
			keysState[0] = true;
		else
			keysState[0] = false;
	}
	else if (key == GLFW_KEY_S){
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
			keysState[1] = true;
		else
			keysState[1] = false;
	}
	else if (key == GLFW_KEY_A){
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
			keysState[2] = true;
		else
			keysState[2] = false;
	}
	else if (key == GLFW_KEY_D){
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
			keysState[3] = true;
		else
			keysState[3] = false;
	}
}

void mouse_callback(GLFWwindow*, int k, int action, int){
	if (k == GLFW_MOUSE_BUTTON_LEFT){
		if (action == GLFW_PRESS || action == GLFW_REPEAT){
			glfwGetCursorPos(window, &cx, &cy);
			keysState[4] = true;
		}
		else
			keysState[4] = false;
	}
}

INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT){
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4);
	window = glfwCreateWindow(1024, 768, "test", 0, 0);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);
	glfwMakeContextCurrent(window);
	glewInit();

	try{
		Scene scene(10);
		DefaultShader* shader = new DefaultShader;
		shader->setViewport(glm::ivec4(0, 0, 1024, 768));
		shader->addLight(glm::vec3(0.25f, 1, 0.5));

		scene.bindShader(shader);
		scene.addObject(0, { loadModel("Models/skybox2.ply"),
			loadTexture("Textures/skybox.bmp") }, SkyBox);
		scene.addObject(1, { loadModel("Models/dungeon.ply"),
			loadTexture("Textures/brick8.bmp") });
		scene.addObject(4, { loadModel("Models/cubeUnwarp.ply"),
			loadTexture("Textures/brick.bmp") });

		scene.move(4, glm::vec3(1, 1, 1));

		while (!glfwWindowShouldClose(window)){
			if (keysState[0])
				camPos -= glm::normalize(camTarget)*0.1f;
			if (keysState[1])
				camPos += glm::normalize(camTarget)*0.1f;
			if (keysState[2])
				aa -= 4.f;
			if (keysState[3])
				aa += 4.f;
			if (keysState[4]){
				double x, y;
				glfwGetCursorPos(window, &x, &y);
				camPos += glm::vec4((float)(cx - x) / 50.f, 0, (float)(cy - y) / 50.f, 0)*
					glm::rotate(aa, glm::vec3(0, 1, 0));
				cx = x;
				cy = y;
			}

			camTarget = glm::vec4(0, 0, 4, 1)* glm::rotate(aa, glm::vec3(0, 1, 0));
			shader->setCamera(glm::vec3(camPos) + glm::vec3(0, 8, 0) + glm::vec3(camTarget),
				glm::vec3(camPos));

			scene.draw();
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}
	catch (std::exception& e){
		MessageBoxA(0, e.what(), 0, 0);
	}
	scene.releaseObject(0);
	scene.releaseObject(1);
	scene.releaseObject(4);
	glfwDestroyWindow(window);
	glfwTerminate();
}