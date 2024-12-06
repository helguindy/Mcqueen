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
#include <cstdlib>
#include <ctime>




#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int lives = 6;
float initialCarPosZ = 0.0f;


// Camera position and orientation variables
float cameraX = 0.0f, cameraY = 7.0f, cameraZ = 20.0f; // Initial position
float lookAtX = 0.0f, lookAtY = 0.0f, lookAtZ = 0.0f;  // Look-at point
float carPosX = 0.0f;
float carPosY = 0.0f;
float carPosZ = 0.0f;
bool isJumping = false;  // Flag to track if the car is jumping
float jumpHeight = 2.0f; // The height the car will jump
float jumpSpeed = 2.1f;  // Speed of the jump
float initialY = 0.0f;   // Store the initial Y position of the car
float timeElapsed = 0.0f;    // Tracks time in the game
float sunIntensity = 0.7f;  // Base intensity of the sunlight
float intensityVariation = 0.3f; // Amplitude of intensity change
float timeSpeed = 0.05f;    // Speed of time progression
bool gameOver = false; // Flag to track game over state
bool timeOver = false;
//float moveSpeed = 1.1f;
float normalSpeed = 1.0f; // Normal speed of the car
float increasedSpeed = 2.2f; // Increased speed after Z = 600
float moveSpeed = normalSpeed; // Initial move speed
const int numCoins = 10; // Number of coins
float coinPositions[numCoins][3]; // Array to store coin positions (X, Y, Z)
const int numTaxis = 7; // Number of taxis
const int numPoliceCars = 7; // Number of police cars
float taxiPositions[numTaxis][3]; // Array to store taxi positions (X, Y, Z)
float policeCarPositions[numPoliceCars][3]; // Array to store police car positions (X, Y, Z)
int score = 0; // Initial score
bool scoreDoubled = false; // Flag to indicate if the score has been doubled
bool gameWin = false; // Flag to track game win state
float jumpProgress = 0.0f; // Progress of the jump (0 to 1)

struct Car {
	float x, y, z;
	float alpha;
	bool fading;
};

// Initialize your car arrays
Car taxis[numTaxis];
Car policeCars[numPoliceCars];




int timer = 60; // Countdown timer in seconds


void playCollisionSound() {
	PlaySound(TEXT("C:\\Users\\Habiba Elguindy\\Downloads\\assignment2\\OpenGL3DTemplate\\bgsong.wav"), NULL, SND_ASYNC);
}

void playBackgroundMusic() {
	// Open the background music file
	mciSendString("open \"C:\\Users\\Habiba Elguindy\\Downloads\\assignment2\\OpenGL3DTemplate\\bgsong.wav\" type mpegvideo alias bgMusic", NULL, 0, NULL);

	// Play the music in a loop
	mciSendString("play bgMusic repeat", NULL, 0, NULL);
}

void updateGame(int value) {
	if (timer > 0 && !gameWin) {
		timer--; // Decrement timer by 1 second
	}
	else {
		if (!gameWin) { // Only set timeOver if the game has not been won 
			timeOver = true; // Game Over when timer reaches 0 
		}
	}

	if (timer % 2 == 0) {
		score += 10; // Increase score by 10 every 2 seconds
	}

	// Request a redraw to update the displayed timer and score
	glutPostRedisplay();

	// Register the timer callback again for the next second
	if (!timeOver && !gameWin) {
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

void generateCoinPositions() {
	for (int i = 0; i < numCoins; ++i) {
		coinPositions[i][0] = static_cast<float>(rand() % 41 - 30); // X: -30 to 30
		coinPositions[i][1] = 1.0f;                                // Y: 0
		coinPositions[i][2] = static_cast<float>(rand() % 1201);    // Z: 0 to 1200
	}
}

void generateCarPositions() {
	for (int i = 0; i < numTaxis; ++i) {
		taxiPositions[i][0] = static_cast<float>(rand() % 71 - 50); // X: -40 to 40
		taxiPositions[i][1] = 0.0f;                                 // Y: 0
		taxiPositions[i][2] = static_cast<float>(rand() + 5 % 1201);    // Z: 0 to 1200
	}
	for (int i = 0; i < numPoliceCars; ++i) {
		policeCarPositions[i][0] = static_cast<float>(rand() % 81 - 40); // X: -40 to 40
		policeCarPositions[i][1] = 0.0f;                                 // Y: 0
		policeCarPositions[i][2] = static_cast<float>(rand() % 1201);    // Z: 0 to 1200
	}
}

void checkCoinCollisions() {
	float coinCollisionRadius = 6.0f; // Adjust as needed for collision detection

	for (int i = 0; i < numCoins; ++i) {
		float dx = carPosX - coinPositions[i][0];
		float dz = carPosZ - coinPositions[i][2];

		if (dx * dx + dz * dz < coinCollisionRadius * coinCollisionRadius) {
			// Collision with a coin detected
			score *= 2; // Double the score

			// Remove the coin or mark it as collected
			coinPositions[i][0] = 100000; // Move the coin out of view
		}
	}
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
void setCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0, 16.0 / 9.0, 1.0, 50.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Calculate the angle to the camera for first-person view
	float deltaX = cameraX - carPosX;
	float deltaZ = cameraZ - carPosZ;
	float angleToCamera = atan2(deltaX, deltaZ) * 180.0f / M_PI; // Convert to degrees

	switch (viewMode) {
		{ case 0: // Third-person view (behind the car) 
			gluLookAt(carPosX - sin(angleToCamera * M_PI / 180.0f) * 20.0f, carPosY + 7.0f, carPosZ - cos(angleToCamera * M_PI / 180.0f) * 20.0f, carPosX, carPosY, carPosZ, 0.0, 1.0, 0.0);
		}
		break;
	case 1: // Top view (above the car, looking down) 
		gluLookAt(carPosX, carPosY + 30.0f, carPosZ, carPosX, carPosY, carPosZ, 0.0, 0.0, -1.0);  
		break;
	case 2: // Side view (beside the car) 
		gluLookAt(carPosX + 30.0f, carPosY + 7.0f, carPosZ, carPosX, carPosY, carPosZ, 0.0, 1.0, 0.0); // Up vector 
	break; 
	case 3: // Front view (in front of the car) 
		gluLookAt(carPosX, carPosY + 7.0f, carPosZ - 30.0f, carPosX, carPosY, carPosZ, 0.0, 1.0, 0.0); // Up vector
		break;
	case 4: // First-person view
		gluLookAt(carPosX, carPosY + 6.5f, carPosZ, // Camera at car position
			carPosX - sin(angleToCamera * M_PI / 180.0f), carPosY + 6.5f, carPosZ + cos(angleToCamera * M_PI / 180.0f), // Looking in the opposite direction of the car
			0.0, 1.0, 0.0); // Up vector
		break;
	}
}


void specialKeyboard(int key, int x, int y) {
	if (!timeOver && !gameWin) {
		if (carPosZ > 600) { moveSpeed = increasedSpeed; }
		else { moveSpeed = normalSpeed; 
		}
		switch (key) {
		case GLUT_KEY_DOWN:    // Up arrow key
			carPosZ -= moveSpeed; // Move car forward along the Z-axis
			if (carPosZ >= 1500) { 
				gameWin = true;
			}
			break;
		case GLUT_KEY_UP:  // Down arrow key
			carPosZ += moveSpeed; 
			if (carPosZ >= 1500) {
				gameWin = true;
			} // Move car backward along the Z-axis
			break;
		case GLUT_KEY_RIGHT:  // Left arrow key
			carPosX -= moveSpeed; // Move car left along the X-axis
			cameraX += (rand() % 5 - 2) * 0.1f; 
			cameraY += (rand() % 5 - 2) * 0.1f;
			break;
		case GLUT_KEY_LEFT: // Right arrow key
			carPosX += moveSpeed; // Move car right along the X-axis
			cameraX += (rand() % 5 - 2) * 0.1f;
			cameraY += (rand() % 5 - 2) * 0.1f;
			break;
		}
	}

	// Update camera position to follow the car
	cameraX = carPosX;
	cameraZ = carPosZ + 20.0f; // Maintain a fixed distance behind the car

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
	initialCarPosZ = carPosZ;

	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
Model_3DS model_gem;
Model_3DS model_flag;
Model_3DS model_taxi;
Model_3DS model_redcar;
Model_3DS model_policecar;
Model_3DS model_sky; // Add this with other model declarations
//Model_3DS model_mud;
// Textures
GLTexture tex_ground;
GLTexture tex_coin;
GLTexture tex_flag;  // Add this line

//GLTexture tex_sky;



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

void RenderGround()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

	// Set a large rectangle to simulate an infinite ground
	float groundSize =5000.0f;  // Large ground size for the visible area
	float textureRepeat = 500.0f; // Texture repetition factor

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

	float collisionRadius = 7.0f;
	float bounceDistance = 10.0f;

	// Check collisions with taxis
	for (int i = 0; i < numTaxis; ++i) {
		if (distance(carPosX, carPosZ, taxiPositions[i][0], taxiPositions[i][2]) < collisionRadius) {
			if (lives > 0) {
				playCollisionSound();
				lives--;
				float deltaX = carPosX - taxiPositions[i][0]; 
				float deltaZ = carPosZ - taxiPositions[i][2]; 
				float length = sqrt(deltaX * deltaX + deltaZ * deltaZ); // Normalize the direction and apply the bounce 
				deltaX /= length; 
				deltaZ /= length; 
				carPosX += deltaX * bounceDistance; 
				carPosZ += deltaZ * bounceDistance;
				taxis[i].fading = true;
				if (lives == 0) {
					gameOver = true;
				}
			}
		}
	}

	// Check collisions with police cars
	for (int i = 0; i < numPoliceCars; ++i) {
		if (distance(carPosX, carPosZ, policeCarPositions[i][0], policeCarPositions[i][2]) < collisionRadius) {
			if (lives > 0) {
				playCollisionSound();
				lives--;
				float deltaX = carPosX - taxiPositions[i][0];
				float deltaZ = carPosZ - taxiPositions[i][2];
				float length = sqrt(deltaX * deltaX + deltaZ * deltaZ); // Normalize the direction and apply the bounce 
				deltaX /= length;
				deltaZ /= length;
				carPosX += deltaX * bounceDistance;
				carPosZ += deltaZ * bounceDistance;
				policeCars[i].fading = true;
				if (lives == 0) {
					gameOver = true;
				}
			}
		}
	}
}

//void RenderSkyBox() {
//	glDisable(GL_LIGHTING);  // Disable lighting for the skybox
//	glEnable(GL_TEXTURE_2D); // Enable 2D texturing
//	glBindTexture(GL_TEXTURE_2D, tex_sky); // Bind the skybox texture
//
//	glPushMatrix();
//	glTranslated(carPosX, carPosY, carPosZ); // Center the skybox on the car
//	GLUquadricObj* qobj = gluNewQuadric();
//	gluQuadricTexture(qobj, true);
//	gluQuadricNormals(qobj, GL_SMOOTH);
//	gluSphere(qobj, 1000.0, 100, 100); // Large enough to encompass the scene
//	gluDeleteQuadric(qobj);
//	glPopMatrix();
//
//	glDisable(GL_TEXTURE_2D);
//	glEnable(GL_LIGHTING);  // Re-enable lighting for other objects
//}
void setGoldMaterial() {
	// Gold material properties
	GLfloat gold_ambient[] = { 0.24725f, 0.1995f, 0.0745f, 1.0f };
	GLfloat gold_diffuse[] = { 0.75164f, 0.60648f, 0.22648f, 1.0f };
	GLfloat gold_specular[] = { 0.628281f, 0.555802f, 0.366065f, 1.0f };
	GLfloat gold_shininess[] = { 51.2f };

	glMaterialfv(GL_FRONT, GL_AMBIENT, gold_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, gold_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, gold_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, gold_shininess);
}
void initCars() {
	for (int i = 0; i < numTaxis; ++i) {
		taxis[i].x = taxiPositions[i][0];
		taxis[i].y = taxiPositions[i][1];
		taxis[i].z = taxiPositions[i][2];
		taxis[i].alpha = 1.0f;
		taxis[i].fading = false;
	}

	for (int i = 0; i < numPoliceCars; ++i) {
		policeCars[i].x = policeCarPositions[i][0];
		policeCars[i].y = policeCarPositions[i][1];
		policeCars[i].z = policeCarPositions[i][2];
		policeCars[i].alpha = 1.0f;
		policeCars[i].fading = false;
	}
}


void myDisplay(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setCamera();

	if (gameOver) {
		renderGameOverScreen();
	}
	else if (gameWin) {
		// Display "Game Win" message and score
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glColor3f(0.0f, 1.0f, 0.0f); // Green "Game Win" text
		glRasterPos2f(WIDTH / 2 - 50, HEIGHT / 2);
		char* winMessage = "You Win!";
		for (char* c = winMessage; *c != '\0'; c++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
		}
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
	}
	else {
		// Handle jumping logic
		if (isJumping) {
			carPosY += jumpSpeed;
			if (carPosY >= initialY + jumpHeight) {
				isJumping = false; // Stop jumping when the maximum height is reached
			}
		}
		else if (carPosY > initialY) {
			carPosY -= jumpSpeed; // Bring the car back down
		}

		// Draw Ground
		RenderGround();

		timeElapsed += timeSpeed;
		float currentIntensity = sunIntensity + intensityVariation * sin(timeElapsed);

		GLfloat lightIntensity[] = { currentIntensity, currentIntensity, currentIntensity, 1.0f };
		GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);

		// Draw car model
		float deltaX = cameraX - carPosX;
		float deltaZ = cameraZ - carPosZ;
		float angleToCamera = atan2(deltaX, deltaZ) * 180.0f / M_PI;

		float modelOffset = 0.0f; // Adjust this value to rotate the car, use 180 to face the other way around
		glPushMatrix();
		glTranslatef(carPosX, carPosY, carPosZ); // Move the car to its position
		glRotatef(angleToCamera + modelOffset, 0.0f, 1.0f, 0.0f);  // Rotate the car to face the camera
		glScalef(3.0, 3.0, 3.0);  // Scale the car uniformly to make it bigger
		model_redcar.Draw();      // Draw the car
		glPopMatrix();

		// Draw coin model
		for (int i = 0; i < numCoins; ++i) {
			glPushMatrix();
			glScalef(0.2, 0.2, 0.2);
			glTranslatef(coinPositions[i][0], coinPositions[i][1], coinPositions[i][2]);
			model_coin.Draw(); // Draw the coin
			glPopMatrix();
		}

		for (int i = 0; i < numTaxis; ++i) {
			glPushMatrix();
			glTranslatef(taxiPositions[i][0], taxiPositions[i][1], taxiPositions[i][2]);
			glRotatef(-90.f, 0, 1, 0); // Rotate to make the car stand on its wheels
			glScalef(0.059, 0.059, 0.059); // Scale the car uniformly to make it bigger
			model_taxi.Draw(); // Draw the taxi
			glPopMatrix();
		}

		for (int i = 0; i < numPoliceCars; ++i) {
			glPushMatrix();
			glTranslatef(policeCarPositions[i][0], policeCarPositions[i][1], policeCarPositions[i][2]);
			glRotatef(-90.f, 0, 1, 0); // Rotate to make the car stand on its wheels
			glScalef(0.09, 0.09, 0.09); // Scale the car uniformly to make it bigger
			model_policecar.Draw();
			glPopMatrix();
		}

		// Draw flag model
		glPushMatrix();
		glTranslatef(0, 0, 600); // Adjust Y translation to lift the car above the ground if necessary
		glRotatef(-90.f, 0, 1, 0); // Rotate around the X-axis to make the car stand on its wheels
		glScalef(0.09, 0.09, 0.09);  // Scale the car uniformly to make it bigger
		model_flag.Draw();
		glPopMatrix();

		//---------------------
		glPushMatrix();
		glEnable(GL_TEXTURE_2D);  // Enable texturing
		glBindTexture(GL_TEXTURE_2D, tex_flag.texture[0]);  // Bind the flag texture

		glTranslatef(0, 0, 600);
		glRotatef(-90.f, 0, 1, 0);
		glScalef(0.09, 0.09, 0.09);
		model_flag.Draw();

		glDisable(GL_TEXTURE_2D);  // Disable texturing after drawing
		glPopMatrix();
		//------------------------------

		//// Draw mud model
		//glPushMatrix();
		//glTranslatef(3, 0, 600); // Adjust Y translation to lift the car above the ground if necessary
		//glRotatef(-90.f, 0, 1, 0); // Rotate around the X-axis to make the car stand on its wheels
		//glScalef(0.09, 0.09, 0.09);  // Scale the car uniformly to make it bigger
		//model_mud.Draw();
		//glPopMatrix();

		// Set up orthographic projection for text rendering
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0.0, WIDTH, 0.0, HEIGHT);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		// Render the environment text at the top
		glDisable(GL_LIGHTING);
		if (carPosZ < 600) {
			renderText(30, 550, "Environment 1", 0.3f); // Adjust position if necessary
		}
		else {
			renderText(30, 550, "Environment 2", 0.3f); // Adjust position if necessary
		}
		glEnable(GL_LIGHTING);

		// Restore previous projection and modelview matrices
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

		// Additional text rendering
		float distanceTraveledZ = carPosZ - initialCarPosZ;
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0.0, WIDTH, 0.0, HEIGHT);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		std::string timerText = "Time Left: " + std::to_string(timer);
		std::string scoreText = "Score: " + std::to_string(score);
		std::string distanceText = "Distance Traveled: " + std::to_string(distanceTraveledZ);
		renderText(20, 60, timerText, 0.1f);
		renderText(20, 90, scoreText, 0.1f);
		renderText(20, 120, distanceText, 0.1f);

		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		
		UpdateCarLights();
		checkCoinCollisions(); // Check for coin collisions
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

	if (timeOver && !gameWin) {
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glColor3f(1.0f, 1.0f, 1.0f);
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


void myMouse(int button, int state, int x, int y)
{
	y = HEIGHT - y;  // Adjust y for proper orientation

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		// Toggle between First-Person (4) and Third-Person (0)
		viewMode = (viewMode == 4) ? 0 : 4; // Assuming 4 is First-Person, 0 is Third-Person
		setCamera();  // Update the camera to reflect the new view
		glutPostRedisplay();  // Redraw the scene to update the camera
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (!isJumping && carPosY == initialY) {
			isJumping = true; 
			jumpProgress = 0.0f; // Reset jump progress 
		}
	}
}



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




void LoadAssets()
{
	// Loading Model files
	model_house.Load("Models/car/car.3DS");
	model_tree.Load("Models/tree/Tree1.3ds");
	model_coin.Load("Models/coin3.3ds");
	model_flag.Load("Models/flagg.3ds");
	model_taxi.Load("Models/taxi.3ds");
	model_redcar.Load("Models/redcar/redcar.3DS");
	model_policecar.Load("Models/police.3ds");
	model_gem.Load("Models/diamond.3ds");//zbtet
	//model_mud.Load("Models/mud.3ds");
	// Loading texture files
	tex_coin.Load("Textures/coin.bmp");
	tex_ground.Load("Textures/ground.bmp");
	tex_flag.Load("Textures/flagg.bmp");

	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
}

void main(int argc, char** argv)
{

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT); //hi

	glutInitWindowPosition(100, 150);
	generateCoinPositions(); // Generate initial coin positions
	generateCarPositions(); // Generate initial car positions

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
// //coins double score ################################################
//collesion with any collectible and the car ##################################
// odstecle in environment 1 cars #####################################
// collision with any obstical lose one life  and make a sound effect #################################
// player has 3 lives ############################################################
// if player lose all lifes game end  ##################################################################
//set collectables and obstecalles position ################### 
// 
// 
// 
// 
// 
// rahma
// camera shake when  the car get left or right for 1 sec ##############################
// when i exceed the flag speed up ##########################################
//display distance  ###############################################
//z<100 1k first environment ################################
//z=100 flage  ####################################################
//z>100 display environment 2 ########################################
//z=2oo 2k finish line game win timer stop 
//  set camera and their animations 
// game win ##############################################
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