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
void deleteMesh();

int numHorizon = 36;
int numVertical = 18;
bool isNormalOn = false;

vec3 p[36][18];
vec3 eye(2, 2.2, 2);
vec3 center(0, 0, 0);
vec3 up(0, 1, 0);    


vec4 lightInitialP(0.8, 0.9, 0.8, 1);

float AXIS_LENGTH = 2;
float AXIS_LINE_WIDTH = 2;

GLfloat bgColor[4] = { 1,1,1,1 };

bool smooth = true;

int selection = 0;

bool pause = true;

float timeStep = 1.0 / 120;
float period = 4.0;

int frame = 0;

bool rotationLight = false;
bool rotationObject = false;
bool lightOn[3];
bool attenuation = false;
float thetaModel = 0;
float thetaLight[3];
bool exponent;
float exponentInitial = 0.0;
float exponentValue = exponentInitial;
float exponentNorm = exponentValue / 128.0;
bool cutoff = false;
float cutoffMax = 60;
float cutoffInitial = 40.0;
float cutoffValue = cutoffInitial;
float cutoffNorm = cutoffValue / cutoffMax;

int nVertices = 0, nFaces = 0, nEdges = 0;
glm::vec3* vertex = NULL;
glm::vec3* vnormal = NULL;
glm::vec3* fnormal = NULL;
int* face[3] = { NULL, NULL, NULL };

float time;

void reinitialize()
{
	frame = 0;
	lightOn[0] = true; // Turn on only the point light
	lightOn[1] = false;
	lightOn[2] = false;
	thetaModel = 0;
	for (int i = 0; i < 3; i++)
		thetaLight[i] = 0;
	exponentValue = exponentInitial;
	exponentNorm = exponentValue / 128.0;
	cutoffValue = cutoffInitial;
	cutoffNorm = cutoffValue / cutoffMax;
}

void computeRotation(const vec3& a, const vec3& b, float& theta, vec3& axis)
{
	axis = cross(a, b);
	float sinTheta = length(axis);
	float cosTheta = dot(a, b);
	theta = atan2(sinTheta, cosTheta) * 180 / M_PI;
}

void animate() {
	frame += 1;
	if (rotationLight)
	{
		for (int i = 0; i < 3; i++)
			if (lightOn[i]) thetaLight[i] += 4 / period; // degree
		if (lightOn[2] && exponent)
			exponentNorm += radians(10.0 / period) / M_PI;
		exponentValue = 128.0 * (acos(cos(exponentNorm * M_PI)) / M_PI);
		if (lightOn[2] && cutoff)
			cutoffNorm += radians(4.0 / period) / M_PI;
		cutoffValue = cutoffMax * (acos(cos(cutoffNorm * M_PI)) / M_PI);
	}
}

int main(int argc, char* argv[]) {
	GLFWwindow * window = initializeOpenGL(argc, argv, bgColor);
	if (window == NULL) return -1;
	
	glm::vec4 o(0.6, 0.6, 0.0, 1.0); //중심점
	glm::vec4 p0(0.6, 0.8, 0.0, 1.0);
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
GLUquadricObj* sphere = NULL;
GLUquadricObj* cylinder = NULL;
GLUquadricObj* cone = NULL;

void init() {
	reinitialize();
	sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricNormals(sphere, GLU_SMOOTH);
	gluQuadricOrientation(sphere, GLU_OUTSIDE);
	gluQuadricTexture(sphere, GL_FALSE);
	cylinder = gluNewQuadric();
	gluQuadricDrawStyle(cylinder, GLU_FILL);
	gluQuadricNormals(cylinder, GLU_SMOOTH);
	gluQuadricOrientation(cylinder, GLU_OUTSIDE);
	gluQuadricTexture(cylinder, GL_FALSE);
	cone = gluNewQuadric();
	gluQuadricDrawStyle(cone, GLU_FILL);
	gluQuadricNormals(cone, GLU_SMOOTH);
	gluQuadricOrientation(cone, GLU_OUTSIDE);
	gluQuadricTexture(cone, GL_FALSE);
	
	
	cout << "Keyboard input : n for draw the vertex normal vectors" << endl;
	cout << "Keyboard input : p for point light" << endl;
	cout << "Keyboard input : d for directional light" << endl;
	cout << "Keyboard input : s for spot light" << endl;
	cout << "Keyboard input : t for time-varying shininess coefficient in specular reflection" << endl;
	cout << "Keyboard input : space start/stop" << endl;		
	
}

void quit() {
	
	gluDeleteQuadric(sphere);
	gluDeleteQuadric(cylinder);
	gluDeleteQuadric(cone);
	// Delete mesh
	deleteMesh();
	
}

void deleteMesh()
{
	if (vertex) { delete[] vertex; vertex = NULL; }
	if (vnormal) { delete[] vnormal; vnormal = NULL; }
	if (fnormal) { delete[] fnormal; fnormal = NULL; }
	if (face[0]) { delete[] face[0]; face[0] = NULL; }
	if (face[1]) { delete[] face[1]; face[1] = NULL; }
	if (face[2]) { delete[] face[2]; face[2] = NULL; }
}

void drawSphere(float radius, int slices, int stacks)
{	
	gluSphere(sphere, radius, slices, stacks);
}
// Draw a cylinder using a GLU quadric
void drawCylinder(float radius, float height, int slices, int stacks)
{	
	gluCylinder(cylinder, radius, radius, height, slices, stacks);
}
void drawCone(float radius, float height, int slices, int stacks)
{
	gluCylinder(cone, 0, radius, height, slices, stacks);
}

void drawWireFrame() {
	glPushMatrix();
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
	glPopMatrix();
}

void drawQuads() {	
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);
	glBegin(GL_QUADS); 
	
	for (int i = 0; i < numHorizon; i++) {
		for (int j = 0; j < numVertical; j++) {			
			vec3 p1(p[i][(j + 1) % 18].x - p[i][j].x, p[i][(j + 1) % 18].y - p[i][j].y, p[i][(j + 1) % 18].z - p[i][j].z);
			vec3 p2(p[(i + 1) % 36][j].x - p[i][j].x, p[(i + 1) % 36][j].y - p[i][j].y, p[(i + 1) % 36][j].z - p[i][j].z);
			vec3 normalVector = cross(p2, p1);
			vec3 middleVector = p[i][j] + p[i][(j + 1) % 18] + p[(i + 1) % 36][j] + p[(i + 1) % 36][(j + 1) % 18];

			glNormal3f(normalVector.x, normalVector.y, normalVector.z);

			glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);
			glVertex3f(p[i][(j + 1) % 18].x, p[i][(j + 1) % 18].y, p[i][(j + 1) % 18].z);
			glVertex3f(p[(i + 1) % 36][(j + 1) % 18].x, p[(i + 1) % 36][(j + 1) % 18].y, p[(i + 1) % 36][(j + 1) % 18].z);
			glVertex3f(p[(i + 1) % 36][j].x, p[(i + 1) % 36][j].y, p[(i + 1) % 36][j].z);
		}
	}

	glEnd();
	
	glDisable(GL_POLYGON_OFFSET_FILL);
	//
	//if (isNormalOn)
	//	drawNormalVector();
	glPopMatrix();
}

void drawNormalVector() {	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (int i = 0; i < numHorizon; i++) {
		for (int j = 0; j < numVertical; j++) {
			vec3 p1(p[i][(j + 1) % 18].x - p[i][j].x, p[i][(j + 1) % 18].y - p[i][j].y, p[i][(j + 1) % 18].z - p[i][j].z);
			vec3 p2(p[(i + 1) % 36][j].x - p[i][j].x, p[(i + 1) % 36][j].y - p[i][j].y, p[(i + 1) % 36][j].z - p[i][j].z);
			vec3 normalVector1= cross(p2, p1);
			////////////////////////////////////////////////////////////////
			vec3 p3(p[(i + 1) % 36][(j + 1) % 18].x - p[i][(j + 1) % 18].x, 
				    p[(i + 1) % 36][(j + 1) % 18].y - p[i][(j + 1) % 18].y,
				    p[(i + 1) % 36][(j + 1) % 18].z - p[i][(j + 1) % 18].z);
			
			vec3 p4(p[i][(j + 2) % 18].x - p[i][(j + 1) % 18].x,
				    p[i][(j + 2) % 18].y - p[i][(j + 1) % 18].y,
				    p[i][(j + 2) % 18].z - p[i][(j + 1) % 18].z);
			vec3 normalVector2 = cross(p2, p1);
			////////////////////////////////////////////////////////////////
			vec3 p5(p[(i + 2) % 36][(j + 1) % 18].x - p[(i + 1) % 36][(j + 1) % 18].x, 
				    p[(i + 2) % 36][(j + 1) % 18].y - p[(i + 1) % 36][(j + 1) % 18].y,
				    p[(i + 2) % 36][(j + 1) % 18].z - p[(i + 1) % 36][(j + 1) % 18].z);

			vec3 p6(p[(i + 1) % 36][(j + 2) % 18].x - p[(i + 1) % 36][(j + 1) % 18].x,
				    p[(i + 1) % 36][(j + 2) % 18].y - p[(i + 1) % 36][(j + 1) % 18].y,
				    p[(i + 1) % 36][(j + 2) % 18].z - p[(i + 1) % 36][(j + 1) % 18].z);
			vec3 normalVector3 = cross(p2, p1);
			////////////////////////////////////////////////////////////////
			vec3 p7(p[(i + 2) % 36][j].x - p[(i + 1) % 36][j].x, 
				    p[(i + 2) % 36][j].y - p[(i + 1) % 36][j].y,
				    p[(i + 2) % 36][j].z - p[(i + 1) % 36][j].z);

			vec3 p8(p[(i + 1) % 36][(j + 1) % 18].x - p[(i + 1) % 36][j].x,
				    p[(i + 1) % 36][(j + 1) % 18].y - p[(i + 1) % 36][j].y,
				    p[(i + 1) % 36][(j + 1) % 18].z - p[(i + 1) % 36][j].z);
			vec3 normalVector4 = cross(p2, p1);
			////////////////////////////////////////////////////////////////
			vec3 normalVector = (normalVector1 + normalVector2 + normalVector3 + normalVector4);			
			vec3 middleVector = p[(i + 1) % 36][(j + 1) % 18];
			glBegin(GL_LINES);
			glColor3f(0.0f, 0.0f, 0.0f);			
			glVertex3f(middleVector.x, middleVector.y, middleVector.z);			
			glVertex3f(
				2*normalVector.x + middleVector.x,
				2*normalVector.y + middleVector.y,
				2*normalVector.z + middleVector.z
			);
		}
	}
	glEnd();
}

void drawArrow(const vec3& p, bool tailOnly)
{
	// Make it possible to change a subset of material parameters
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	// Common material
	GLfloat mat_specular[4] = { 1, 1, 1, 1 };
	GLfloat mat_shininess = 25;
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	// Transformation
	glPushMatrix();
	glTranslatef(p.x, p.y, p.z);
	if (!tailOnly)
	{
		float theta;
		vec3 axis;
		computeRotation(vec3(0, 0, 1), vec3(0, 0, 0) - vec3(p), theta, axis);
		glRotatef(theta, axis.x, axis.y, axis.z);
	}
	// Tail sphere
	float arrowTailRadius = 0.05;
	glColor3f(1, 0, 0); // ambient and diffuse
	drawSphere(arrowTailRadius, 16, 16);
	if (!tailOnly)
	{
		// Shaft cylinder
		float arrowShaftRadius = 0.02;
		float arrowShaftLength = 0.2;
		glColor3f(0, 1, 0);
		drawCylinder(arrowShaftRadius, arrowShaftLength, 16, 5);
		// Head cone
		float arrowheadHeight = 0.09;
		float arrowheadRadius = 0.06;
		glTranslatef(0, 0, arrowShaftLength + arrowheadHeight);
		glRotatef(180, 1, 0, 0);
		glColor3f(0, 0, 1); // ambient and diffuse
		drawCone(arrowheadRadius, arrowheadHeight, 16, 5);
	}
	glPopMatrix();
	// For convential material setting				
	glDisable(GL_COLOR_MATERIAL);
}


void setupLight(const vec4& p, int i)
{
	GLfloat ambient[4]{ 0.1, 0.1, 0.1, 1 };
	GLfloat diffuse[4] = { 1.0, 1.0, 1.0, 1 };
	GLfloat specular[4] = { 1.0, 1.0, 1.0, 1 };
	glLightfv(GL_LIGHT0 + i, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0 + i, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0 + i, GL_POSITION, value_ptr(p));
	// Attenuation for the point light
	if (i == 0 && attenuation)
	{
		glLightf(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, 1.0);
		glLightf(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, 0.1);
		glLightf(GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, 0.05);
	}
	else { // Default value
		glLightf(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, 1.0);
		glLightf(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, 0.0);
		glLightf(GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, 0.0);
	}
	if (i == 2) // Spot light
	{
		vec3 spotDirection = -vec3(p);		
		glLightfv(GL_LIGHT0 + i, GL_SPOT_DIRECTION, value_ptr(spotDirection));
		glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, cutoffValue ); // (0, 90]
		glLightf(GL_LIGHT0 + i, GL_SPOT_EXPONENT, exponentValue); // [0, 128]
	}
	else { // Point and distant light. 
		   // 180 to turn off cutoff when it was used as a spot light.
		glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, 180); // uniform light distribution
	}
}

void setupColoredMaterial(const vec3& color)
{
	// Material
	GLfloat mat_ambient[4] = { 0.1, 0.1, 0.1, 1 };
	GLfloat mat_diffuse[4] = { color[0], color[1], color[2], 1 };
	GLfloat mat_specular[4] = { 0.5, 0.5, 0.5, 1 };
	GLfloat mat_shininess = 100;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

void drawSpotLight(const vec3& p, float cutoff)
{

	glPushMatrix();
	glTranslatef(p.x, p.y, p.z);
	float theta;
	vec3 axis;
	
	computeRotation(vec3(0, 0, 1), vec3(0, 0, 0) - vec3(p), theta, axis);
	glRotatef(theta, axis.x, axis.y, axis.z);
	// Color	

	setupColoredMaterial(vec3(0, 0, 1));
	// tan(cutoff) = r/h
	float h = 0.15;
	float r = h * tan(radians(cutoff));
	drawCone(r, h, 16, 5);
	// Color
	setupColoredMaterial(vec3(1, 0, 0));
	// Apex
	float apexRadius = 0.06 * (0.5 + exponentValue / 128.0);
	drawSphere(apexRadius, 16, 16);
	glPopMatrix();
}

void setupWhiteShinyMaterial()
{
	// Material
	GLfloat mat_ambient[4]{0.1, 0.1, 0.1, 1 };
	GLfloat mat_diffuse[4]{ 1, 1, 1, 1 };
	GLfloat mat_specular[4] = { 1, 1, 1, 1 };
	GLfloat mat_shininess = 100;		
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);		
}

void render(GLFWwindow* window) {
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/*const vec3& eye = eyeFrontView;
	const vec3& up = upFrontView;*/
	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);
	
	glDisable(GL_LIGHTING);
	drawAxes(AXIS_LENGTH, AXIS_LINE_WIDTH*dpiScaling);		

	//if (smooth) glShadeModel(GL_SMOOTH);
	//else glShadeModel(GL_FLAT);
	glShadeModel(GL_SMOOTH);
	
	
	//float theta = frame * 4 / period;
	glm::vec3 axis(0, 1, 0);
	//glRotatef(theta, axis.x, axis.y, axis.z);
	drawWireFrame();
	glEnable(GL_LIGHTING);
	// Set up the lights
	
	vec4 lightP[3];	
	for (int i = 0; i < 3; i++)
	{
		// Just turn off the i-th light, if not lit
		if (!lightOn[i]) { glDisable(GL_LIGHT0 + i); continue; }
		// Turn on the i-th light
		glEnable(GL_LIGHT0 + i);
		// Dealing with the distant light
		lightP[i] = lightInitialP;
		if (i == 1) lightP[i].w = 0;
		mat4 R = rotate(mat4(1.0), radians(thetaLight[i]), axis);
		// Lights rotate around the center of the world coordinate system			
		lightP[i] = R * lightP[i];
		// Set up the i-th light
		setupLight(lightP[i], i);				
	}
	
	// Draw the geometries of the lights
	setupWhiteShinyMaterial();
	drawQuads();
	for (int i = 0; i < 3; i++)
	{
		if (!lightOn[i]) continue;
		if (i == 2) drawSpotLight(lightP[i], cutoffValue);
		else drawArrow(lightP[i], i == 0);
	}
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
		case GLFW_KEY_Q:
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE); break;						
		case GLFW_KEY_N: 
			if (isNormalOn)
				isNormalOn = false;
			else
				isNormalOn = true;
			break;		
		case GLFW_KEY_SPACE: rotationLight = !rotationLight; pause = !pause; break;
		case GLFW_KEY_P: lightOn[0] = !lightOn[0]; break;
		case GLFW_KEY_D: lightOn[1] = !lightOn[1]; break;
		case GLFW_KEY_S: lightOn[2] = !lightOn[2]; break;		
		case GLFW_KEY_T: cutoff = !cutoff; break;
		}
	}
}
