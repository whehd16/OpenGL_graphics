#include "glSetup.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#define M_PI 3.1415926535897932384626433832795

using namespace glm;
using namespace std;

void init();
void quit();
void render(GLFWwindow* window);
void keyboard(GLFWwindow* window, int key, int code, int action, int mods);
void drawNormalVector();
void drawTwoSide();

int numHorizon = 36;
int numVertical = 18;
bool isNormalOn = false;

vec3 p[36][18];
vec3 onePoint(2, 2, 0);
vec3 eye(7, 7, 7);
vec3 center(0, 0, 0);
vec3 up(0, 1, 0);

//vec4 light(5.0, 5.0, 0.0, 1);

float AXIS_LENGTH = 2;
float AXIS_LINE_WIDTH = 2;

GLfloat bgColor[4] = { 1,1,1,1 };

bool smooth = true;

int selection = 0;

//GLUquadricObj* sphere = NULL;
//GLUquadricObj* cylinder = NULL;

bool pause = true;

float timeStep = 1.0 / 120;
float period = 4.0;

int frame = 0;

void animate() {
	frame += 1;
}

int main(int argc, char* argv[]) {
	GLFWwindow * window = initializeOpenGL(argc, argv, bgColor);
	if (window == NULL) return -1;
	
	glm::vec4 o(2.0, 2.0, 0.0, 1.0); //중심점
	glm::vec4 p0(3.0, 2.0, 0.0, 1.0);
	glm::vec4 axis(0.0, 0.0, 1.0, 1.0); // z축 중심으로 회전
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
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();		
		
		float now = glfwGetTime();
		float delta = now - previous;
		previous = now;

		elapsed += delta;

		if (elapsed > timeStep) {
			if (!pause)
				animate();
			elapsed = 0;
		}

		render(window);
		glfwSwapBuffers(window);		
	}

	quit();
	
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void init() {
			
	cout << "Keyboard input : 1 for draw 36 x 18 data points" << endl;
	cout << "Keyboard input : 2 for draw the wireframe only" << endl;
	cout << "Keyboard input : 3 for draw the quads only" << endl;
	cout << "Keyboard input : 4 for draw the quads and the wireframe" << endl;
	cout << "Keyboard input : 5 for draw the normal vectors of the polygons" << endl;
	cout << "Keyboard input : 6 for two-sided constant shading with the wireframe" << endl;	
	cout << "Keyboard input : a, s for around the y-axis" << endl;
	cout << "Keyboard input : j, k for around the z-axit " << endl;
	
}

void quit() {

	//deleteMesh();
}

void drawPoints() {		
	for (int i = 0; i < numHorizon; i++) {
		for (int j = 0; j < numVertical; j++) {
			glPointSize(2.0);
			glBegin(GL_POINTS);				
			glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);
		}
	}			
	glEnd();
}

void drawWireFrame() {
	glColor3f(0.0f, 0.0f, 0.0f);			
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(1, 1);
	

	glBegin(GL_QUADS);
	for (int i = 0; i < numHorizon; i++) {
		for (int j = 0; j < numVertical; j++) {											
			glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);
			glVertex3f(p[i][ (j+1) % 18 ].x, p[i][(j + 1) % 18].y, p[i][(j + 1) % 18].z);
			glVertex3f(p[(i + 1) % 36][(j + 1) % 18].x, p[(i + 1) % 36][(j + 1) % 18].y, p[(i + 1) % 36][(j + 1) % 18].z);
			glVertex3f(p[(i + 1) % 36][j].x, p[(i + 1) % 36][j].y, p[(i + 1) % 36][j].z);						
		}
	}			
	glEnd();
	glDisable(GL_POLYGON_OFFSET_LINE);
	if (isNormalOn)
		drawNormalVector();
}

void drawQuads() {	
	glColor3f(0.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);
	glBegin(GL_QUADS);

	for (int i = 0; i < numHorizon; i++) {
		for (int j = 0; j < numVertical; j++) {			
			glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);
			glVertex3f(p[i][(j + 1) % 18].x, p[i][(j + 1) % 18].y, p[i][(j + 1) % 18].z);
			glVertex3f(p[(i + 1) % 36][(j + 1) % 18].x, p[(i + 1) % 36][(j + 1) % 18].y, p[(i + 1) % 36][(j + 1) % 18].z);
			glVertex3f(p[(i + 1) % 36][j].x, p[(i + 1) % 36][j].y, p[(i + 1) % 36][j].z);
		}
	}

	glEnd();
	
	glDisable(GL_POLYGON_OFFSET_FILL);
	
	if (isNormalOn)
		drawNormalVector();
}



void drawQuadsWithWireframe() {										
	drawQuads();
	drawWireFrame();	
}

void drawTwoSide() {
	vec3 perspective = eye;	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);
	glBegin(GL_QUADS);

	for (int i = 0; i < numHorizon; i++) {
		for (int j = 0; j < numVertical; j++) {
									
			vec3 p1(p[i][(j + 1) % 18].x - p[i][j].x, p[i][(j + 1) % 18].y - p[i][j].y, p[i][(j + 1) % 18].z - p[i][j].z);
			vec3 p2(p[(i + 1) % 36][j].x - p[i][j].x, p[(i + 1) % 36][j].y - p[i][j].y, p[(i + 1) % 36][j].z - p[i][j].z);
			vec3 normalVector = cross(p2, p1);						

			vec3 midPoint = p[i][j] + p[i][(j + 1) % 18] + p[(i + 1) % 36][j] + p[(i + 1) % 36][(j + 1) % 18];

			float result = dot(perspective - midPoint / 4.0f, normalVector);
			
			if (result > 0.0f)
				glColor3f(0.0f, 0.0f, 1.0f);
			else			
				glColor3f(1.0f, 0.0f, 0.0f);
																	
			glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);
			glVertex3f(p[i][(j + 1) % 18].x, p[i][(j + 1) % 18].y, p[i][(j + 1) % 18].z);
			glVertex3f(p[(i + 1) % 36][(j + 1) % 18].x, p[(i + 1) % 36][(j + 1) % 18].y, p[(i + 1) % 36][(j + 1) % 18].z);
			glVertex3f(p[(i + 1) % 36][j].x, p[(i + 1) % 36][j].y, p[(i + 1) % 36][j].z);			
		}
	}
	
	glEnd();	
	glDisable(GL_POLYGON_OFFSET_FILL);	
	drawWireFrame();	
}

void drawNormalVector() {	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (int i = 0; i < numHorizon; i++) {
		for (int j = 0; j < numVertical; j++) {
			vec3 p1(p[i][(j + 1) % 18].x - p[i][j].x, p[i][(j + 1) % 18].y - p[i][j].y, p[i][(j + 1) % 18].z - p[i][j].z);
			vec3 p2(p[(i + 1) % 36][j].x - p[i][j].x, p[(i + 1) % 36][j].y - p[i][j].y, p[(i + 1) % 36][j].z - p[i][j].z);
			vec3 normalVector = cross(p2, p1);
			vec3 middleVector = p[i][j] + p[i][(j + 1) % 18] + p[(i + 1) % 36][j] + p[(i + 1) % 36][(j + 1) % 18];			
			glBegin(GL_LINES);
			glColor3f(0.0f, 0.0f, 0.0f);
			glVertex3f(middleVector.x / 4, middleVector.y / 4, middleVector.z / 4);			
			glVertex3f(
				3 * normalVector.x + middleVector.x / 4,
				3 * normalVector.y + middleVector.y / 4,
				3 * normalVector.z + middleVector.z / 4
			);
		}
	}
	glEnd();
}

int nVertices = 0, nFaces = 0, nEdges = 0;
glm::vec3* vertex = NULL;
glm::vec3* vnormal = NULL;
glm::vec3* fnormal = NULL;
int* face[3] = { NULL, NULL, NULL };

void render(GLFWwindow* window) {
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);
	
	glDisable(GL_LIGHTING);
	drawAxes(AXIS_LENGTH, AXIS_LINE_WIDTH*dpiScaling);		

	if (smooth) glShadeModel(GL_SMOOTH);
	else glShadeModel(GL_FLAT);


	float theta = frame * 4 / period;
	glm::vec3 axis(0, 1, 0);
	glRotatef(theta, axis.x, axis.y, axis.z);

	switch (selection) {
	case 0: break;
	case 1: drawPoints(); break;	
	case 2: drawWireFrame(); break;
	case 3: drawQuads(); break;
	case 4: drawQuadsWithWireframe(); break;
	case 6: drawTwoSide(); break;
	}

}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
		case GLFW_KEY_Q:
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE); break;				
		case GLFW_KEY_1: selection = 1; break;
		case GLFW_KEY_2: selection = 2; break;
		case GLFW_KEY_3: selection = 3; break;
		case GLFW_KEY_4: selection = 4; break;
		case GLFW_KEY_5: 
			if (isNormalOn)
				isNormalOn = false;
			else
				isNormalOn = true;
			break;
		case GLFW_KEY_6: selection = 6; break;
		case GLFW_KEY_J: 
			numVertical += 1;
			if (numVertical > 18) {
				numVertical = 18;
			}
			break;
		case GLFW_KEY_K: 
			numVertical -= 1;
			if (numVertical < 0) {
				numVertical = 0;
			}
			break;
		case GLFW_KEY_A: 
			numHorizon += 1; 
			if (numHorizon > 35)
				numHorizon = 36;
			
			break;
		case GLFW_KEY_S: 
			numHorizon -= 1; 
			if (numHorizon < 0)
				numHorizon = 0;			
			break;
			
		}
	}
}
