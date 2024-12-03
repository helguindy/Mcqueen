#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <iostream>
#include <string>
#include <sstream>
#include <Windows.h>
#include <mmsystem.h>
#include <cstdio>


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int lives = 6;

// Camera position and orientation variables
float cameraX = 0.0f, cameraY = 7.0f, cameraZ = 20.0f; // Initial position
float lookAtX = 0.0f, lookAtY = 0.0f, lookAtZ = 0.0f;  // Look-at point
float carPosX = 0.0f;
float carPosY = 0.0f;
float carPosZ = 0.0f;
bool isJumping = false;  // Flag to track if the car is jumping
float jumpHeight = 2.0f; // The height the car will jump
float jumpSpeed = 0.1f;  // Speed of the jump
float initialY = 0.0f;   // Store the initial Y position of the car
float timeElapsed = 0.0f;    // Tracks time in the game
float sunIntensity = 0.7f;  // Base intensity of the sunlight
float intensityVariation = 0.3f; // Amplitude of intensity change
float timeSpeed = 0.05f;    // Speed of time progression
bool gameOver = false; // Flag to track game over state
bool timeOver = false;


int timer = 60; // Countdown timer in seconds
int score = 0;  // Player score

void playCollisionSound() {
	PlaySound(TEXT("C:\\Users\\Habiba Elguindy\\Downloads\\assignment2\\OpenGL3DTemplate\\collectables.wav"), NULL, SND_ASYNC);
}

void playBackgroundMusic() {
	// Open the background music file
	mciSendString("open \"C:\\Users\\Habiba Elguindy\\Downloads\\assignment2\\OpenGL3DTemplate\\bgsong.wav\" type mpegvideo alias bgMusic", NULL, 0, NULL);

	// Play the music in a loop
	mciSendString("play bgMusic repeat", NULL, 0, NULL);
}

void updateGame(int value) {
	if (timer > 0) {
		timer--; // Decrement timer by 1 second
	}
	else {
		timeOver = true;  // Game Over when timer reaches 0
	}

	if (timer % 2 == 0) {
		score += 10; // Increase score by 10 every 2 seconds
	}

	// Request a redraw to update the displayed timer and score
	glutPostRedisplay();

	// Register the timer callback again for the next second
	if (!timeOver) {
		glutTimerFunc(1000, updateGame, 0);
	}
}
void renderText(float x, float y, const std::string& text, float scale) {
	glDisable(GL_LIGHTING);
	glColor3f(1.0f, 1.0f, 1.0f); // Set text color to white
	glRasterPos2f(x, y);

	// Apply scaling
	glPushMatrix();
	glScalef(scale, scale, 1.0f);  // Scale the text by the given factor

	// Render each character in the string
	for (char c : text) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}

	// Restore the previous matrix
	glPopMatrix();
	glEnable(GL_LIGHTING);
}


// Update function to decrement timer and increment score
void update(int value) {
	if (timer > 0) {
		timer--; // Decrement the timer
		score++; // Increment the score for demonstration
	}
	else {
		timeOver = true;
	}

	glutPostRedisplay(); // Redraw the screen
	glutTimerFunc(1000, update, 0); // Call this function every second
}

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
void setCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0, 16.0 / 9.0, 1.0, 50.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Adjust camera position based on view mode
	
	switch (viewMode) {
	case 0: // Default perspective view
		gluLookAt(cameraX, cameraY, cameraZ, lookAtX, lookAtY, lookAtZ, 0.0, 1.0, 0.0);
		break;
	case 1: // Top view
		gluLookAt(0.0, 30.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0);
		break;
	case 2: // Side view
		gluLookAt(30.0, 7.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		break;
	case 3: // Front view
		gluLookAt(0.0, 7.0, 30.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		break;
	}
}





void specialKeyboard(int key, int x, int y) {
	const float moveSpeed = 1.1f; // Speed of the car movement
	if (!timeOver) {
		switch (key) {
		case GLUT_KEY_UP:    // Up arrow key
			carPosZ -= moveSpeed; // Move car forward along the Z-axis
			break;
		case GLUT_KEY_DOWN:  // Down arrow key
			carPosZ += moveSpeed; // Move car backward along the Z-axis
			break;
		case GLUT_KEY_LEFT:  // Left arrow key
			carPosX -= moveSpeed; // Move car left along the X-axis
			break;
		case GLUT_KEY_RIGHT: // Right arrow key
			carPosX += moveSpeed; // Move car right along the X-axis
			break;
		}
	}

	setCamera(); // Update the camera to follow the car
	glutPostRedisplay(); // Request display update after movement
}




void setupOrthoProjection(int windowWidth, int windowHeight) {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0f, windowWidth, 0.0f, windowHeight, -1.0f, 1.0f); // 2D orthographic projection
	glMatrixMode(GL_MODELVIEW);
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
Model_3DS model_flag;
Model_3DS model_taxi;
Model_3DS model_redcar;



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
	//*//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	//*//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*//

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

void renderGameOverScreen() {
	glClear(GL_COLOR_BUFFER_BIT); // Clear the screen

	// Set text color
	glColor3f(1.0f, 0.0f, 0.0f); // Red "Game Over" text

	// Render "Game Over" message
	renderText(0.4f, 0.5f, " ", 0.002f);

	glFlush(); // Ensure everything is drawn
}
void renderLives() {
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(1.0f, 0.0f, 0.0f);

	float scale = 1.0f;  // Reduced from 20.0f
	float spacing = 50.0f; // Reduced from 50.0f
	float baseX = 30.0f;  // Reduced from 50.0f
	float baseY = HEIGHT - 30.0f; // Reduced from 50.0f

	for (int i = 0; i < lives; i++) {
		float x = baseX + (i * spacing);
		float y = baseY;

		glBegin(GL_POLYGON);
		for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
			float px = x + scale * 16 * pow(sin(angle), 3);
			float py = y + scale * (13 * cos(angle) - 5 * cos(2 * angle) - 2 * cos(3 * angle) - cos(4 * angle));
			glVertex2f(px, py);
		}
		glEnd();
	}

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}


// Add these global variables at the top
float distance(float x1, float z1, float x2, float z2) {
	return sqrt(pow(x2 - x1, 2) + pow(z2 - z1, 2));
}

// Modify checkCollisions():
void checkCollisions() {
	if (gameOver) return;

	float taxiX = 5, taxiZ = 0;
	//float toktokX = -10, toktokZ = 0;
	float collisionRadius = 3.0f;

	if (distance(carPosX, carPosZ, taxiX, taxiZ) < collisionRadius) {
		if (lives > 0) {
			playCollisionSound();
			lives--;
			carPosX = 0;
			carPosZ = 0;
			if (lives == 0) {
				gameOver = true;
			}
		}
	}


}





//=======================================================================
// Display Function
void myDisplay(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setCamera();

	if (gameOver) {
		// Display Game Over Text
		renderGameOverScreen();
	}
	else {
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

		float modelOffset = 180.0f; // Adjust this value if needed (try 90, 180, or 270)

		glPushMatrix();
		glTranslatef(carPosX, carPosY, carPosZ); // Move the car to its position
		glRotatef(angleToCamera + modelOffset, 0.0f, 1.0f, 0.0f);  // Rotate the car to face the camera

		glScalef(3.0, 3.0, 3.0);  // Scale the car uniformly to make it bigger
		model_redcar.Draw();          // Draw the car
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

		// Draw flag model
		glPushMatrix();
		glTranslatef(15, 3, 0); // Adjust Y translation to lift the car above the ground if necessary
		glRotatef(-90.f, 0, 1, 0); // Rotate around the X-axis to make the car stand on its wheels
		glScalef(0.09, 0.09, 0.09);  // Scale the car uniformly to make it bigger
		model_flag.Draw();
		glPopMatrix();

		// Draw flag model
		glPushMatrix();
		glTranslatef(5, 0, 1); // Adjust Y translation to lift the car above the ground if necessary
		glRotatef(-90.f, 0, 1, 0); // Rotate around the X-axis to make the car stand on its wheels
		glScalef(0.059, 0.059, 0.059);  // Scale the car uniformly to make it bigger
		model_taxi.Draw();
		glPopMatrix();

		// Draw redcar model
		//glPushMatrix();
		//glTranslatef(5, 0, 1); // Adjust Y translation to lift the car above the ground if necessary
		//glRotatef(-90.f, 0, 1, 0); // Rotate around the X-axis to make the car stand on its wheels
		//glScalef(3, 3,3);  // Scale the car uniformly to make it bigger
		//model_redcar.Draw();
		//glPopMatrix();


		//sky box
		//glPushMatrix();
		//GLUquadricObj* qobj;
		//qobj = gluNewQuadric();
		//glTranslated(0, 50, 0);
		//glRotated(90, 1, 0, 1);
		//glBindTexture(GL_TEXTURE_2D, tex);
		//gluQuadricTexture(qobj, true);
		//gluQuadricNormals(qobj, GL_SMOOTH);
		//gluSphere(qobj, 100, 100, 100);
		//gluDeleteQuadric(qobj);
		//glPopMatrix();

		UpdateCarLights();


		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0.0, 800.0, 0.0, 600.0); // Adjust based on window dimensions
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		std::string timerText = "Time Left: " + std::to_string(timer);
		std::string scoreText = "Score: " + std::to_string(score);

		renderText(20, 60, timerText, 0.1f);  // You can adjust the position and size as needed
		renderText(20, 90, scoreText, 0.1f);

	}

	if (gameOver) {
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glColor3f(1.0f, 0.0f, 0.0f);
		glRasterPos2f(WIDTH / 2 - 50, HEIGHT / 2);
		char* message = "Game Over!";
		for (char* c = message; *c != '\0'; c++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
		}

		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
	}
	renderLives();
	checkCollisions();

	if (timeOver) {
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glColor3f(1.0f, 0.0f, 0.0f);
		glRasterPos2f(WIDTH / 2 - 50, HEIGHT / 2);
		char* message = "Time Over!";
		for (char* c = message; *c != '\0'; c++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
		}

		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
	}
	renderLives();
	checkCollisions();

	glutSwapBuffers();
}



//=======================================================================
// Keyboard Function
//=======================================================================
// Function to handle key presses for camera control
void keyboard(unsigned char key, int x, int y) {
	const float moveStep = 1.0f;

	switch (key) {
	case 'w': cameraZ -= moveStep; break;
	case 's': cameraZ += moveStep; break;
	case 'a': cameraX -= moveStep; break;
	case 'd': cameraX += moveStep; break;
	case 'q': cameraY += moveStep; break;
	case 'e': cameraY -= moveStep; break;
	case '1': viewMode = 0; break;
	case '2': viewMode = 1; break;
	case '3': viewMode = 2; break;
	case '4': viewMode = 3; break;
	case 27: exit(0); break;
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
	model_flag.Load("Models/flag.3ds");
	model_taxi.Load("Models/taxi.3ds");
	model_redcar.Load("Models/redcar/redcar.3DS");

	// Loading texture files
	tex_ground.Load("Textures/ground.bmp");

	//loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
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
	playBackgroundMusic();
	glutDisplayFunc(myDisplay);
	glutTimerFunc(1000, update, 0); // Start the update loop

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

// demiana
// texture gems cars muds finish line 
// //coins double score
//collesion with any collectible and the car 
// odstecle in environment 1 cars
// collision with any obstical lose one life  and make a sound effect #################################
// player has 3 lives ############################################################
// if player lose all lifes game end  ##################################################################
//set collectables and obstecalles position 
// 
// 
// 
// 
// 
// rahma
// first and third person view and switch by rigth click
// camera shake when  the car get left or right for 1 sec 
// when i exceed the flag speed up
//display distance 
//z<100 1k first environment
//z=100 flage 
//z>100 display environment 2 
//z=2oo 2k finish line game win timer stop 
//  set camera and their animations
// game win
// 
// 
// 
// 
// habiba
// make two environment 
// start point 
//end point
// car lights
//environment 1
//display score increase by one every two seconds ####################################
//timer  1 min ##########################################
// game over #######################################
// environment 2 new obsistecale slow dowm for 10 sec and new collectable  
//