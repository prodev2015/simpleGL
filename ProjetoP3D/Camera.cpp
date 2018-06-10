#pragma region include
using namespace std;

#define GLEW_STATIC
#include <GL\glew.h>

#define GLFW_USE_DWM_SWAP_INTERVAL
#include <GLFW\glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.hpp"
#pragma endregion

#define WIDTH 1920
#define HEIGHT 1080

glm::vec3 cameraPos = glm::vec3(6.0f, 4.0f, 6.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 2.5f, 0.0f);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

glm::mat4 controlViewMatrix;

float currentFrame;
float lastFrame = 0.0f;
float deltaTime = 0.0f;

float speed = 1.5f; // 3 units / second
float verticalSpeed = 3.0f;
float radius = 8.0f;

float camX;
float camZ;

float angle = 0.0f;


void computeMatrixFromInputs(GLFWwindow * window) {
	
	currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos.y += verticalSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos.y -=  verticalSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		angle -= speed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		angle += speed * deltaTime;
	}
		
	cameraPos.x = sin(angle) * radius;
	cameraPos.z = cos(angle) * radius;

	if (cameraPos.y < 0.0f) cameraPos.y = 0.0f;
	else if (cameraPos.y > 7.0f) cameraPos.y = 7.0f;
	
	controlViewMatrix = glm::lookAt(
		cameraPos,           // Camera is here
		cameraTarget, // and looks here : at the same position, plus "direction"
		up                 // Head is up (set to 0,-1,0 to look upside-down)
	);
}

glm::mat4 getViewMatrix() {
	return controlViewMatrix;
}

