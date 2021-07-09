
#include "glSetup.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#define M_PI 3.1415926535897932384626433832795
using namespace glm;
using namespace std;

void init();
void animate();
void render(GLFWwindow* window);
void keyboard(GLFWwindow* window, int key, int code, int action, int mods);

GLchar image[1][512][512][3];
vec3 normal[36][18];
vec3 p[36][18];
vec3 eye(2.5, 2, 2.5);
vec3 center(0, 0, 0);
vec3 up(0, 1, 0);

vec4 light(1.0, 1.5, 1.5, 0);

float AXIS_LENGTH = 2;
float AXIS_LINE_WIDTH = 2;

GLfloat bgColor[4] = { 1,1,1,1 };

bool pause = true;

float timeStep = 1.0 / 120;
float period = 4.0;

int frame = 0;

int texNum = 0;
void animate() {
	frame += 1;
}

int main(int argc, char* argv[]) {
	GLFWwindow* window = initializeOpenGL(argc, argv, bgColor);
	if (window == NULL)
		return -1;
	
	glm::vec4 o(0.6, 0.6, 0.0, 1.0); 
	glm::vec4 p0(0.6, 0.8, 0.0, 1.0);
	glm::vec4 axis(0.0, 0.0, 1.0, 1.0);
	glm::mat4 transMatrix;
	glm::mat4 transReturnMatrix;
	glm::mat4 rotateMatrix;

	for (int i = 0; i < 36; i++) {
		rotateMatrix = glm::rotate(glm::mat4(1.0f), (float)glm::radians(10.f), glm::vec3(0.0, 1.0, 0.0));
		p0 = rotateMatrix * p0;
		o = rotateMatrix * o;

		axis = rotateMatrix * axis;

		transMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-o.x, -o.y, -o.z));
		transReturnMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(o.x, o.y, o.z));

		for (int j = 0; j < 18; j++) {
			rotateMatrix = glm::rotate(glm::mat4(1.0f), (float)glm::radians(20.f*j), glm::vec3(axis.x, axis.y, axis.z));
			p[i][j] = transReturnMatrix * rotateMatrix * transMatrix* p0;
		}
	}
	
	glfwSetKeyCallback(window, keyboard);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	reshape(window, windowW, windowH);

	init();

	float previous = glfwGetTime();
	float elapsed = 0;
	while (!glfwWindowShouldClose(window)){
		glfwPollEvents();
		render(window);
		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void loadMarbleTexture() {
	FILE *file[1];
	fopen_s(&file[0], "marble.raw", "rb");
	for (int h = 0; h < 512; h++) {
		fread(image[0][h], 3, 512, file[0]);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, image[0]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	fclose(file[0]);
}

void loadWoodTexture() {
	FILE *file[1];
	fopen_s(&file[0], "wood.raw", "rb");
	for (int h = 0; h < 512; h++) {
		fread(image[0][h], 3, 512, file[0]);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, image[0]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	fclose(file[0]);
}

void loadCheckTexture() {
	FILE *file[1];
	fopen_s(&file[0], "check.raw", "rb");
	for (int h = 0; h < 512; h++) {
		fread(image[0][h], 3, 512, file[0]);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, image[0]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	fclose(file[0]);
}

GLuint texID[3];

void init() {
	glEnable(GL_TEXTURE_2D);
	glGenTextures(3, texID);

	glBindTexture(GL_TEXTURE_2D, texID[0]);
	loadMarbleTexture();

	glBindTexture(GL_TEXTURE_2D, texID[1]);
	loadWoodTexture();

	glBindTexture(GL_TEXTURE_2D, texID[2]);
	loadCheckTexture();

	float* nomalVec = new float[3];
	for (int i = 0; i < 36; i++) {
		for (int j = 0; j < 18; j++) {
			float line1[3] = { p[i % 36][j % 18].x - p[i % 36][(j + 1) % 18].x, p[i % 36][j % 18].y - p[i % 36][(j + 1) % 18].y, p[i % 36][j % 18].z - p[i % 36][(j + 1) % 18].z };
			float line2[3] = { p[(i + 1) % 36][(j + 1) % 18].x - p[i % 36][(j + 1) % 18].x, p[(i + 1) % 36][(j + 1) % 18].y - p[i % 36][(j + 1) % 18].y, p[(i + 1) % 36][(j + 1) % 18].z - p[i % 36][(j + 1) % 18].z };

			nomalVec[0] = line1[1] * line2[2] - line1[2] * line2[1];
			nomalVec[1] = line1[2] * line2[0] - line1[0] * line2[2];
			nomalVec[2] = line1[0] * line2[1] - line1[1] * line2[0];

			float size = sqrt((nomalVec[0] * nomalVec[0]) + (nomalVec[1] * nomalVec[1]) + (nomalVec[2] * nomalVec[2]));

			nomalVec[0] /= size;
			nomalVec[1] /= size;
			nomalVec[2] /= size;

			normal[i % 36][j % 18].x += nomalVec[0];
			normal[i % 36][j % 18].y += nomalVec[1];
			normal[i % 36][j % 18].z += nomalVec[2];

			normal[i % 36][(j + 1) % 18].x += nomalVec[0];
			normal[i % 36][(j + 1) % 18].y += nomalVec[1];
			normal[i % 36][(j + 1) % 18].z += nomalVec[2];

			normal[(i + 1) % 36][j % 18].x += nomalVec[0];
			normal[(i + 1) % 36][j % 18].y += nomalVec[1];
			normal[(i + 1) % 36][j % 18].z += nomalVec[2];

			normal[(i + 1) % 36][(j + 1) % 18].x += nomalVec[0];
			normal[(i + 1) % 36][(j + 1) % 18].y += nomalVec[1];
			normal[(i + 1) % 36][(j + 1) % 18].z += nomalVec[2];
		}
	}

	for (int i = 0; i < 36; i++)
	{
		for (int j = 0; j < 18; j++)
		{
			double size = sqrt(normal[i][j].x * normal[i][j].x + normal[i][j].y * normal[i][j].y + normal[i][j].z * normal[i][j].z);
			normal[i][j].x /= size;
			normal[i][j].y /= size;
			normal[i][j].z /= size;
		}
	}


	cout << "Keyboard input: 1 for marble texture" << endl;
	cout << "Keyboard input: 2 for wood texture" << endl;
	cout << "Keyboard input: 3 for check texture" << endl;
}

void setupLight() {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	GLfloat ambient[4] = { 1.0, 1.0, 1.0, 1 };
	GLfloat diffuse[4] = { 1.0, 1.0, 1.0, 1 };
	GLfloat specular[4] = { 1.0, 1.0, 1.0, 1 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, value_ptr(light));
}

void setupMaterial() {	
	glEnable(GL_COLOR_MATERIAL);

	GLfloat mat_ambient[4] = { 0.1, 0.1, 0.1, 1 };
	GLfloat mat_specular[4] = { 0.5, 0.5, 0.5, 1 };
	GLfloat mat_shininess = 25;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

void drawTexturedCube() {
	glBindTexture(GL_TEXTURE_2D, texID[texNum]);
	for (int i = 0; i < 36; i++) {
		for (int j = 0; j < 18; j++) {
			
			vec3 p1(p[i][(j + 1) % 18].x - p[i][j].x, p[i][(j + 1) % 18].y - p[i][j].y, p[i][(j + 1) % 18].z - p[i][j].z);
			vec3 p2(p[(i + 1) % 36][j].x - p[i][j].x, p[(i + 1) % 36][j].y - p[i][j].y, p[(i + 1) % 36][j].z - p[i][j].z);
			vec3 normalVector = cross(p2, p1);
			vec3 middleVector = p[i][j] + p[i][(j + 1) % 18] + p[(i + 1) % 36][j] + p[(i + 1) % 36][(j + 1) % 18];
			
			glColor3f(1, 1, 1);
			glBegin(GL_QUADS);
			glTexCoord2f((float)i / 36, (float)j / 18);
			glNormal3f(normal[i % 36][j % 18].x, normal[i % 36][j % 18].y, normal[i % 36][j % 18].z);								
			glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);

			glTexCoord2f((float)i / 36, (float)(j + 1) / 18);
			glNormal3f(normal[i % 36][(j + 1) % 18].x, normal[i % 36][(j + 1) % 18].y, normal[i % 36][(j + 1) % 18].z);
			glVertex3f(p[i][(j + 1) % 18].x, p[i][(j + 1) % 18].y, p[i][(j + 1) % 18].z);
			
			glTexCoord2f((float)(i + 1) / 36, (float)(j + 1) / 18);
			glNormal3f(normal[(i + 1) % 36][(j + 1) % 18].x, normal[(i + 1) % 36][(j + 1) % 18].y, normal[(i + 1) % 36][(j + 1) % 18].z);
			glVertex3f(p[(i + 1) % 36][(j + 1) % 18].x, p[(i + 1) % 36][(j + 1) % 18].y, p[(i + 1) % 36][(j + 1) % 18].z);
			
			glTexCoord2f((float)(i + 1) / 36, (float)j / 18);
			glNormal3f(normal[(i + 1) % 36][j % 18].x, normal[(i + 1) % 36][j % 18].y, normal[(i + 1) % 36][j % 18].z);
			glVertex3f(p[(i + 1) % 36][j].x, p[(i + 1) % 36][j].y, p[(i + 1) % 36][j].z);
			glEnd();
		}	
	}		
}

void render(GLFWwindow* window) {
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);
	drawAxes(AXIS_LENGTH, AXIS_LINE_WIDTH*dpiScaling);
	setupLight();
	setupMaterial();

	drawTexturedCube();
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
		case GLFW_KEY_Q:
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE); break;
		case GLFW_KEY_1: texNum = 0; break;
		case GLFW_KEY_2: texNum = 1; break;
		case GLFW_KEY_3: texNum = 2; break;
		}
	}
}


