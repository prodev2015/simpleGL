#pragma region include
using namespace std;

#define GLEW_STATIC
#include <GL\glew.h>

#define GLFW_USE_DWM_SWAP_INTERVAL
#include <GLFW\glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Controls.hpp"
#pragma endregion

#define WIDTH 1920
#define HEIGHT 1080

glm::vec3 position = glm::vec3(0, 0, 3);
float horizontalAngle = 3.14f;
float verticalAngle = 0.0f;

double currentFrame;
double lastFrame;
double deltaTime;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 1;

double xpos, ypos;

glm::mat4 controlViewMatrix;

void computeMatrixFromInputs(GLFWwindow * window) {
	currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	glfwGetCursorPos(window, &xpos, &ypos);
	glfwSetCursorPos(window, WIDTH / 2, HEIGHT /2);
	
	// Compute new orientation
	horizontalAngle -= mouseSpeed * deltaTime * (float)(WIDTH / 2 - xpos);
	verticalAngle += mouseSpeed * deltaTime * (float)(HEIGHT / 2 - ypos);

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);

	glm::vec3 up = glm::cross(right, direction);

	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position += direction * (float)deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position -= direction * (float)deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position += right * (float)deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position -= right * (float)deltaTime * speed;
	}

	controlViewMatrix = glm::lookAt(
		position,           // Camera is here
		position + direction, // and looks here : at the same position, plus "direction"
		up                  // Head is up (set to 0,-1,0 to look upside-down)
	);
}

glm::mat4 getViewMatrix() {
	return controlViewMatrix;
}


