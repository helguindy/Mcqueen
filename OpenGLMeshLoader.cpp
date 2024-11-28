#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <iostream>


#include <cstdio>

// Camera position and orientation variables
float cameraX = 0.0f, cameraY = 7.0f, cameraZ = 20.0f; // Initial position
float lookAtX = 0.0f, lookAtY = 0.0f, lookAtZ = 0.0f;  // Look-at point
float carPosX = 0.0f;
float carPosZ = 0.0f;
bool isJumping = false;  // Flag to track if the car is jumping
float jumpHeight = 2.0f; // The height the car will jump
float jumpSpeed = 0.1f;  // Speed of the jump
float initialY = 0.0f;   // Store the initial Y position of the car
float timeElapsed = 0.0f;    // Tracks time in the game
float sunIntensity = 0.7f;  // Base intensity of the sunlight
float intensityVariation = 0.3f; // Amplitude of intensity change
float timeSpeed = 0.05f;    // Speed of time progression



void InitCarLights() {
	// Enable Light Source 1
	glEnable(GL_LIGHT1);

	// Define Light source 1 properties
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);

	// Set spotlight properties for headlights
	GLfloat spotDirection[] = { 0.0f, -0.5f, -1.0f }; // Pointing downward and forward
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDirection);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 30.0f);       // Spotlight cone angle
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0f);      // Spotlight intensity distribution
}

// Update the car lights' position and direction
void UpdateCarLights() {
	GLfloat lightPosition[] = { carPosX, 1.0f, carPosZ, 1.0f }; // Slightly above the car
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
}


// View mode: 0 = perspective (default), 1 = top view, 2 = side view, 3 = front view
int viewMode = 0;

// Function to set up the camera
// Function to set up the camera
// Function to set up the camera
void setCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0, 16.0 / 9.0, 1.0, 50.0);  // Adjust perspective view

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set camera based on viewMode
	switch (viewMode) {
	case 0: // First-Person View
		// Camera is at the player's position, looking at where the player is facing
		gluLookAt(cameraX, cameraY, cameraZ, lookAtX, lookAtY, lookAtZ, 0.0, 1.0, 0.0);
		break;
	case 1: // Third-Person View
		// Camera is positioned behind the player with an offset
		gluLookAt(cameraX + 5.0f, cameraY + 2.0f, cameraZ - 5.0f, lookAtX, lookAtY, lookAtZ, 0.0, 1.0, 0.0);
		break;
	}
}


void specialKeyboard(int key, int x, int y) {
	// Handle car movement using the arrow keys
	const float moveSpeed = 1.0f;  // Speed of the car movement
	switch (key) {
	case GLUT_KEY_UP:    // Up arrow key
		carPosZ -= moveSpeed;  // Move car forward
		break;
	case GLUT_KEY_DOWN:  // Down arrow key
		carPosZ += moveSpeed;  // Move car backward
		break;
	case GLUT_KEY_LEFT:  // Left arrow key
		carPosX -= moveSpeed;  // Move car left
		break;
	case GLUT_KEY_RIGHT: // Right arrow key
		carPosX += moveSpeed;  // Move car right
		break;
	}
	glutPostRedisplay();
}






// Initialization function
void init() {
	glEnable(GL_DEPTH_TEST);

	// Set up lighting (optional)
	GLfloat ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat lightPosition[] = { -7.0f, 10.0f, 10.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}


int WIDTH = 1280;
int HEIGHT = 720;

GLuint tex;
char title[] = "3D Model Loader Sample";

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 100;

class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	//==========================================================================//
	// Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
	// Here we are overloading the += operator to add a given value to all vector coordinates.        //
	//================================================================================================//
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
};

Vector Eye(20, 5, 20);
Vector At(0, 0, 0);
Vector Up(0, 1, 0);

int cameraZoom = 0;

// Model Variables
Model_3DS model_house;
Model_3DS model_tree;
Model_3DS model_coin;

// Textures
GLTexture tex_ground;


//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);

	// Define Light source 0 ambient light
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// Define Light source 0 diffuse light
	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	// Define Light source 0 Specular light
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************************************************************************//

	InitLightSource();
	InitCarLights();


	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	// Set up lighting (optional)
	GLfloat ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat lightPosition[] = { -7.0f, 10.0f, 10.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

//=======================================================================
// Render Ground Function
//=======================================================================
void RenderGround()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

	// Set a large rectangle to simulate an infinite ground
	float groundSize = 500.0f;  // Large ground size for the visible area
	float textureRepeat = 50.0f; // Texture repetition factor

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0); // Normal pointing upwards

	// Texture coordinates are set to repeat over the large ground area
	glTexCoord2f(0, 0); glVertex3f(-groundSize, 0, -groundSize); // Bottom-left
	glTexCoord2f(textureRepeat, 0); glVertex3f(groundSize, 0, -groundSize); // Bottom-right
	glTexCoord2f(textureRepeat, textureRepeat); glVertex3f(groundSize, 0, groundSize); // Top-right
	glTexCoord2f(0, textureRepeat); glVertex3f(-groundSize, 0, groundSize); // Top-left

	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING); // Re-enable lighting for other objects

	glColor3f(1, 1, 1); // Reset material color to white
}

//=======================================================================
// Display Function
void myDisplay(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setCamera();

	// Draw Ground
	RenderGround();

	timeElapsed += timeSpeed;
	float currentIntensity = sunIntensity + intensityVariation * sin(timeElapsed);

	// Update the sunlight intensity
	GLfloat lightIntensity[] = { currentIntensity, currentIntensity, currentIntensity, 1.0f };
	GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);

	// Draw car model
// Assuming your camera is at position (cameraX, cameraZ)
	float deltaX = cameraX - carPosX;
	float deltaZ = cameraZ - carPosZ;

	// Calculate the angle between the car and the camera using atan2
	float angleToCamera = atan2(deltaX, deltaZ) * 180.0f / 3.14159f;

	glPushMatrix();
	glTranslatef(carPosX, 0.0f, carPosZ); // Move the car to its position
	glRotatef(angleToCamera, 0, 1, 0);  // Rotate the car to face the camera
	glScalef(10.0, 10.0, 10.0);  // Scale the car uniformly to make it bigger
	model_house.Draw();          // Draw the car
	glPopMatrix();


	// Draw coin model
	glPushMatrix();
	glTranslatef(15, 5, 0); // Adjust Y translation to lift the car above the ground if necessary
	glRotatef(-90.f, 0, 1, 0); // Rotate around the X-axis to make the car stand on its wheels
	glScalef(1.0, 1.0, 1.0);  // Scale the car uniformly to make it bigger
	model_coin.Draw();
	glPopMatrix();


	// Draw coin model
	glPushMatrix();
	glTranslatef(10, 5, 10); // Adjust Y translation to lift the car above the ground if necessary
	glRotatef(-90.f, 0, 1, 0); // Rotate around the X-axis to make the car stand on its wheels
	glScalef(1.0, 1.0, 1.0);  // Scale the car uniformly to make it bigger
	model_coin.Draw();
	glPopMatrix();

	// Draw coin model
	glPushMatrix();
	glTranslatef(10, 25, 0); // Adjust Y translation to lift the car above the ground if necessary
	glRotatef(-90.f, 0, 1, 0); // Rotate around the X-axis to make the car stand on its wheels
	glScalef(1.0, 1.0, 1.0);  // Scale the car uniformly to make it bigger
	model_coin.Draw();
	glPopMatrix();

	// Draw coin model
	glPushMatrix();
	glTranslatef(15, 5, 0); // Adjust Y translation to lift the car above the ground if necessary
	glRotatef(-90.f, 0, 1, 0); // Rotate around the X-axis to make the car stand on its wheels
	glScalef(1.0, 1.0, 1.0);  // Scale the car uniformly to make it bigger
	model_coin.Draw();
	glPopMatrix();

	//sky box
	glPushMatrix();
	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	glTranslated(50, 0, 0);
	glRotated(90, 1, 0, 1);
	glBindTexture(GL_TEXTURE_2D, tex);
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 100, 100, 100);
	gluDeleteQuadric(qobj);
	glPopMatrix();

	UpdateCarLights();

	glutSwapBuffers();
}




//=======================================================================
// Keyboard Function
//=======================================================================
// Function to handle key presses for camera control
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case '1': // Default perspective view
		viewMode = 0;
		break;
	case '2': // Top view
		viewMode = 1;
		break;
	case '3': // Side view
		viewMode = 2;
		break;
	case '4': // Front view
		viewMode = 3;
		break;
	}
	glutPostRedisplay();
}
//=======================================================================
// Motion Function
//=======================================================================
void myMotion(int x, int y)
{
	y = HEIGHT - y;

	if (cameraZoom - y > 0)
	{
		Eye.x += -0.1;
		Eye.z += -0.1;
	}
	else
	{
		Eye.x += 0.1;
		Eye.z += 0.1;
	}

	cameraZoom = y;

	glLoadIdentity();	//Clear Model_View Matrix

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters

	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glutPostRedisplay();	//Re-draw scene 
}

//=======================================================================
// Mouse Function
//=======================================================================
// Mouse Function
void myMouse(int button, int state, int x, int y)
{
	y = HEIGHT - y;  // Adjust y for proper orientation

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		// Toggle between First-Person (0) and Third-Person (1)
		viewMode = (viewMode == 0) ? 1 : 0;
		glutPostRedisplay();  // Redraw the scene to update the camera
	}
}




//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	model_house.Load("Models/car/car.3DS");
	model_tree.Load("Models/tree/Tree1.3ds");
	model_coin.Load("Models/coin.3ds");

	// Loading texture files
	tex_ground.Load("Textures/ground.bmp");

	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
}

//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);

	glutKeyboardFunc(keyboard);

	glutSpecialFunc(specialKeyboard);

	glutMotionFunc(myMotion);

	glutMouseFunc(myMouse);

	glutReshapeFunc(myReshape);

	myInit();

	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}
