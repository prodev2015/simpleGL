#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#pragma region include

using namespace std;
#include <iostream>
#include <vector>

#define GLEW_STATIC
#include <GL\glew.h>

#define GLFW_USE_DWM_SWAP_INTERVAL
#include <GLFW\glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "LoadShaders.hpp"
#include "Controls.hpp"
#include "ObjLoader.hpp"
#pragma endregion

#define WIDTH 1920
#define HEIGHT 1080

void init(void);
void display(void);
glm::vec3 headsUp(bool isHeadUp);
void print_gl_info();
void print_error(int error, const char *description);

GLuint vertexBuffer;
GLuint colorBuffer;
GLuint normalbuffer;
GLuint vertexArrayID;
GLuint programID;
GLuint matrixID;

vector< glm::vec3 > vertices;
vector< glm::vec2 > uvs;
vector< glm::vec3 > normals;

GLFWwindow* window;


double lastTime = glfwGetTime();
int nbFrames = 0;

#pragma region Matrices
//Model Matrix//
glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f); //translation
glm::mat4 translationMatrix = glm::translate(glm::mat4(), translation);

float angleInDegrees = 0; //rotation in degrees
glm::vec3 rotationAxis(0, 0, 0); // axis for rotation
glm::mat4 rotationMatrix = glm::rotate(angleInDegrees, rotationAxis);

float scale = 2; //scale
glm::mat4 scaleMatrix = glm::scale(glm::vec3(scale, scale, scale));

//glm::mat4 modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
glm::mat4 modelMatrix = glm::mat4(1.0f);
//-------------//


//ViewMatrix//
bool isHeadUp = true; //change to false to look upside down

glm::vec3 cameraPosition = glm::vec3(3, 3, 3); // Camera position in World Space
glm::vec3 lookAtPosition = glm::vec3(0, 0, 0); // Position to look at

glm::mat4 viewMatrix = glm::lookAt(cameraPosition, lookAtPosition, headsUp(isHeadUp));
//----------//


//ProjectionMatrix//
float fov = 45; //field of view
float aspectRatio = (float)WIDTH / (float)HEIGHT; //aspectRatio
float near = 0.1f; //near
float far = 100; //far
glm::mat4 projectionMatrix = glm::perspective(glm::radians(fov) , aspectRatio, near, far);




//----------------//


//MVP//
glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
//---//
#pragma endregion

int main(void) {
	

	glewExperimental = true; 

	if (!glfwInit())
	{
		return -1;
	}
								   
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4); 

	window = glfwCreateWindow(WIDTH, HEIGHT, "ProjetoP3D", monitor, NULL);
	if (window == NULL) {
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);

	glewExperimental = true; 
	if (glewInit() != GLEW_OK) {
		return -1;
	}
	
	init();

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	do {
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw
		display();

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

}

void init(void) {
	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	/*
	// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f,-1.0f,-1.0f, // triangle : begin
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, // triangle : end
		1.0f, 1.0f,-1.0f, 
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f, 
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f
	};
	*/
	// One color for each vertex. They were generated randomly.
	static const GLfloat g_color_buffer_data[] = {
		0.583f,  0.771f,  0.014f,
		0.609f,  0.115f,  0.436f,
		0.327f,  0.483f,  0.844f,
		0.822f,  0.569f,  0.201f,
		0.435f,  0.602f,  0.223f,
		0.310f,  0.747f,  0.185f,
		0.597f,  0.770f,  0.761f,
		0.559f,  0.436f,  0.730f,
		0.359f,  0.583f,  0.152f,
		0.483f,  0.596f,  0.789f,
		0.559f,  0.861f,  0.639f,
		0.195f,  0.548f,  0.859f,
		0.014f,  0.184f,  0.576f,
		0.771f,  0.328f,  0.970f,
		0.406f,  0.615f,  0.116f,
		0.676f,  0.977f,  0.133f,
		0.971f,  0.572f,  0.833f,
		0.140f,  0.616f,  0.489f,
		0.997f,  0.513f,  0.064f,
		0.945f,  0.719f,  0.592f,
		0.543f,  0.021f,  0.978f,
		0.279f,  0.317f,  0.505f,
		0.167f,  0.620f,  0.077f,
		0.347f,  0.857f,  0.137f,
		0.055f,  0.953f,  0.042f,
		0.714f,  0.505f,  0.345f,
		0.783f,  0.290f,  0.734f,
		0.722f,  0.645f,  0.174f,
		0.302f,  0.455f,  0.848f,
		0.225f,  0.587f,  0.040f,
		0.517f,  0.713f,  0.338f,
		0.053f,  0.959f,  0.120f,
		0.393f,  0.621f,  0.362f,
		0.673f,  0.211f,  0.457f,
		0.820f,  0.883f,  0.371f,
		0.982f,  0.099f,  0.879f
	};
	

	// Read our .obj file
	bool res = loadOBJ("Model/Iron_Man.obj", vertices, uvs, normals);
	
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexBuffer);
	//Buffer is our vertexbuffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	
	//Color buffer
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
	

	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	//Shaders
	programID = LoadShaders("triangles.vert", "triangles.frag");
	matrixID = glGetUniformLocation(programID, "MVP");


}

void display(void) {
	
	//viewMatrix based on inputs
	computeMatrixFromInputs(window);
	viewMatrix = getViewMatrix();
	mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;

	glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvpMatrix[0][0]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	
	// Attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(
		0,                  
		3,                  
		GL_FLOAT,           
		GL_FALSE,           
		0,                 
		(void*)0            
	);
	
	
	// Attribute buffer : colors
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glVertexAttribPointer(
		1,                                
		3,                                
		GL_FLOAT,                         
		GL_FALSE,                         
		0,                                
		(void*)0                          
	);
	

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	glUseProgram(programID);

	// Draw the triangle
	glDrawArrays(GL_TRIANGLES, 0, vertices.size()); // 12*3 indices starting at 0 -> 12 triangles -> 6 squares
	
	
	
	glDisableVertexAttribArray(0);
	
}

glm::vec3 headsUp(bool isHeadUp) {
	if (isHeadUp) return glm::vec3(0, 1, 0);
		return glm::vec3(0, -1, 0);
}

void print_gl_info() {
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	cout << "\nOpenGL version " << major << '.' << minor << endl;

	const GLubyte *glversion = glGetString(GL_VERSION);
	const GLubyte *glvendor = glGetString(GL_VENDOR);
	const GLubyte *glrenderer = glGetString(GL_RENDERER);
	cout << "\nVersion:  " << glversion << endl <<
		"Vendor:   " << glvendor << endl <<
		"Renderer: " << glrenderer << endl;

	cout << "\nSupported GLSL versions:\n";
	const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	cout << "Higher supported version:\n" << glslversion << endl;
	GLint numglslversions;
	cout << "Other supported versions:\n";
	glGetIntegerv(GL_NUM_SHADING_LANGUAGE_VERSIONS, &numglslversions);
	for (int n = 0; n < numglslversions; n++) {
		cout << glGetStringi(GL_SHADING_LANGUAGE_VERSION, n) << endl;
	}
}

void print_error(int error, const char *description) {
	cout << description << endl;
}

