// Standard C++ library imports
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <typeinfo>
#include <vector>

// OpenGL and GLUT imports
#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

// Import object classes
#include "object.h"

// Static global values for display dimensions
static int DISPLAY_WIDTH = 500;
static int DISPLAY_HEIGHT = 500;
static int DISPLAY_OFFSET_WIDTH;
static int DISPLAY_OFFSET_HEIGHT;

// Static rocket object always in foreground
static Rocket rocket = Rocket(0, 0, 0);

// Static global value for tunnel dimensions
static const float TUNNEL_RADIUS = 5;
static const float TUNNEL_SEGMENT_LENGTH = 25;
static const float TUNNEL_START_Z = 2;
static const float TUNNEL_END_Z = -10;
static const float TUNNEL_POS_INCREMENT = 3;

// Static value and message string for score
static int score;
static char scoreMessage[64] = { '\0' };

// Static vector containing objects in flight path
static std::vector<Object> objects;

// Draw tunnel
void drawTunnel()
{
	// Draw solid tunnel surfaces
	glColor3f(0.1, 0.15, 0.2);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	for (int i = 0; i < 360; i++)
	{
		glBegin(GL_QUADS);
			// Corner at front of tunnel view, no degree offset
			glVertex3f(
				TUNNEL_RADIUS * cos(i * M_PI / 180),
				TUNNEL_RADIUS * sin(i * M_PI / 180),
				floor(rocket.positionZ / TUNNEL_SEGMENT_LENGTH)
					* TUNNEL_SEGMENT_LENGTH
					+ TUNNEL_START_Z * TUNNEL_SEGMENT_LENGTH);

			// Corner at back of tunnel view, no degree offset
			glVertex3f(
				TUNNEL_RADIUS * cos(i * M_PI / 180),
				TUNNEL_RADIUS * sin(i * M_PI / 180),
				floor(rocket.positionZ / TUNNEL_SEGMENT_LENGTH)
					* TUNNEL_SEGMENT_LENGTH
					+ TUNNEL_END_Z * TUNNEL_SEGMENT_LENGTH);

			// Corner at front of tunnel view, one degree off
			glVertex3f(
				TUNNEL_RADIUS * cos((i + 1) * M_PI / 180),
				TUNNEL_RADIUS * sin((i + 1) * M_PI / 180),
				floor(rocket.positionZ / TUNNEL_SEGMENT_LENGTH)
					* TUNNEL_SEGMENT_LENGTH
					+ TUNNEL_END_Z * TUNNEL_SEGMENT_LENGTH);

			// Corner at back of tunnel view, one degree off
			glVertex3f(
				TUNNEL_RADIUS * cos((i + 1) * M_PI / 180),
				TUNNEL_RADIUS * sin((i + 1) * M_PI / 180),
				floor(rocket.positionZ / TUNNEL_SEGMENT_LENGTH)
					* TUNNEL_SEGMENT_LENGTH
					+ TUNNEL_START_Z * TUNNEL_SEGMENT_LENGTH);
		glEnd();
	}

	// Draw lines along tunnel
	glColor3f(0.4, 0.4, 0.4);
	for (int i = 0; i < 360; i += 45)
	{
		glBegin(GL_LINES);
			// Start at front of tunnel view
			glVertex3f(
				TUNNEL_RADIUS * cos(i * M_PI / 180),
				TUNNEL_RADIUS * sin(i * M_PI / 180),
				floor(rocket.positionZ / TUNNEL_SEGMENT_LENGTH)
					* TUNNEL_SEGMENT_LENGTH
					+ TUNNEL_START_Z * TUNNEL_SEGMENT_LENGTH);

			// End at back of tunnel view
			glVertex3f(
				TUNNEL_RADIUS * cos(i * M_PI / 180),
				TUNNEL_RADIUS * sin(i * M_PI / 180),
				floor(rocket.positionZ / TUNNEL_SEGMENT_LENGTH)
					* TUNNEL_SEGMENT_LENGTH
					+ TUNNEL_END_Z * TUNNEL_SEGMENT_LENGTH);
		glEnd();
	}

	// Draw lines perpendicular to tunnel
	for (int i = TUNNEL_START_Z; i >= TUNNEL_END_Z; i--)
	{
		glColor3f(0.4, 0.4, 0.4);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBegin(GL_LINE_STRIP);
			for (int j = 0; j <= 360; j++)
				glVertex3f(
					TUNNEL_RADIUS * cos(j * M_PI / 180),
					TUNNEL_RADIUS * sin(j * M_PI / 180),
					floor(rocket.positionZ / TUNNEL_SEGMENT_LENGTH)
						* TUNNEL_SEGMENT_LENGTH
						+ i * TUNNEL_SEGMENT_LENGTH);
		glEnd();
	}
}

// Position various objects within tunnel
void positionObjects()
{
	// Remove objects passed by rocket
	while (!objects.empty() && objects.at(0).positionZ
		> rocket.positionZ + TUNNEL_SEGMENT_LENGTH)
		objects.erase(objects.begin());

	// Continue generating objects randomly
	if (objects.empty() || objects.back().positionZ - TUNNEL_SEGMENT_LENGTH
		> rocket.positionZ + TUNNEL_END_Z * TUNNEL_SEGMENT_LENGTH)
	{
		// Repeat for 1-3 iterations at each segment
		int iterations = rand() % 3 + 1;
		for (int i = 1; i <= iterations; i++)
		{
			// Generate coordinates to place object
			float x = (rand() % 3 - 1) * 1.6;
			float y = (rand() % 3 - 1) * 1.6;
			float z = rocket.positionZ
				+ TUNNEL_END_Z * TUNNEL_SEGMENT_LENGTH;
			int type = rand() % 3 + 1;

			// Overwrite object at that position if exists
			for (int i = 0; i < objects.size(); i++)
				if (objects.at(i).positionX == x
					&& objects.at(i).positionY == y
					&& objects.at(i).positionZ == z)
					objects.erase(objects.begin() + i);

			// Render specific type of object
			if (type == 1)
				objects.push_back(Coin(x, y, z));
			if (type == 2)
				objects.push_back(Obstacle(x, y, z));
			if (type == 3)
				objects.push_back(Bomb(x, y, z));
		}
	}
}

// Move rocket forward (preliminary for now)
void moveRocket(int val)
{
	rocket.positionZ -= 0.5;

	glutTimerFunc(1000 / 60, moveRocket, 0);
}

// Show score (inverse of rocket z position for now)
void showScore()
{
	// Calculate score, likely move to separate function later
	score = (int)floor(-1 * rocket.positionZ);

	// Position raster text below rocket
	glRasterPos3f(
		rocket.positionX * 0.9 - 1.15
			- 0.15 * floor(log(score) / log(10)),
		rocket.positionY * 0.9 - 2,
		rocket.positionZ + 5);

	// Populate score display message
	memset(scoreMessage, '\0', sizeof(scoreMessage));
	snprintf(scoreMessage, sizeof(scoreMessage),
		"Score: %d", score);

	// Populate raster image to display text
	glColor3f(1, 1, 1);
	for (int i = 0; i < strlen(scoreMessage); i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,
			scoreMessage[i]);
}

// Display function: renders points on screen
void display()
{
	// Reset display before rendering scene
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// Position camera behind rocket
	gluLookAt(
		rocket.positionX / 2, rocket.positionY / 2,
		rocket.positionZ + TUNNEL_SEGMENT_LENGTH,
		0, 0, rocket.positionZ,
		0, 1, 0
	);

	// Tunnel in background
	drawTunnel();

	// Calculate object positions
	positionObjects();

	// Render objects at calculated positions
	for (int i = 0; i < objects.size(); i++)
	{
		
		if (objects.at(i).type == COIN)
			static_cast<Coin&>(objects.at(i)).render();

		if (objects.at(i).type == OBSTACLE)
			static_cast<Obstacle&>(objects.at(i)).render();

		if (objects.at(i).type == BOMB)
			static_cast<Bomb&>(objects.at(i)).render();
	}

	// Render rocket in foreground
	rocket.render();

	// Display score below rocket
	showScore();

	// Flushes buffered commands to display
	glFlush();

	// Redraw points continuously
	glutPostRedisplay();
}

// Keyboard function: handles standard keyboard controls
void keyboard(unsigned char key, int x, int y)
{
	// Perform action depending on character received
	switch (key)
	{
		// Quit program
		case 'q':
		case 'Q':
		case 27:
			exit(0);
			break;
	}
}

// Reshape function: set coordinates on window load/resize
void reshape(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 0, 500);
	glMatrixMode(GL_MODELVIEW);
}

// Special function: handle special keyboard controls
void special(int key, int x, int y)
{
	// Perform action depending on which key pressed
	switch (key)
	{
		// Arrow controls for robot movement
		case GLUT_KEY_UP:
			if (rocket.positionY < TUNNEL_POS_INCREMENT)
				rocket.positionY += TUNNEL_POS_INCREMENT;
			break;
		case GLUT_KEY_DOWN:
			if (rocket.positionY > -1 * TUNNEL_POS_INCREMENT)
				rocket.positionY -= TUNNEL_POS_INCREMENT;
			break;
		case GLUT_KEY_LEFT:
			if (rocket.positionX > -1 * TUNNEL_POS_INCREMENT)
				rocket.positionX -= TUNNEL_POS_INCREMENT;
			break;
		case GLUT_KEY_RIGHT:
			if (rocket.positionX < TUNNEL_POS_INCREMENT)
				rocket.positionX += TUNNEL_POS_INCREMENT;
			break;
	}
}

// Main function: entry point and initialization
int main(int argc, char ** argv)
{
	// GLUT initialization
	glutInit(&argc, argv);

	// Calculate screen bounds to center window
	DISPLAY_OFFSET_WIDTH =
		((glutGet(GLUT_SCREEN_WIDTH) - DISPLAY_WIDTH) / 2);
	DISPLAY_OFFSET_HEIGHT =
		((glutGet(GLUT_SCREEN_HEIGHT) - DISPLAY_HEIGHT) / 2);

	// Set window dimensions and location
	glutInitWindowSize(DISPLAY_WIDTH, DISPLAY_HEIGHT);
	glutInitWindowPosition(DISPLAY_OFFSET_WIDTH,
		DISPLAY_OFFSET_HEIGHT);

	// Create window (main playing area)
	glutCreateWindow("3GC3 - Project");

	// I/O function bindings
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);

	// Initiate rocket movement
	glutTimerFunc(1000 / 60, moveRocket, 0);

	// Main program loop
	glutMainLoop();

	// Exit normally
	return 0;
}