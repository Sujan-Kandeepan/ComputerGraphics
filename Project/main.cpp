// Standard C++ library imports
#include <algorithm>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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

// Local imports
#include "interface.cpp"
#include "material.cpp"
#include "object.h"

// Static rocket object always in foreground
static Rocket rocket = Rocket(0, 0, 0);

// Static global variables for rocket position/movement
enum RocketPosX { X_LEFT = -3, X_MIDDLE = 0, X_RIGHT = 3 };
enum RocketPosY { Y_DOWN = -3, Y_MIDDLE = 0, Y_UP = 3 };
static int oldRocketPosX;
static int oldRocketPosY;
static int newRocketPosX;
static int newRocketPosY;
static int percentRocketMoved;
static int burst;

// Static vector containing objects in flight path
static std::vector<Object> objects;

// Static global value for tunnel dimensions
static const float TUNNEL_RADIUS = 6;
static const float TUNNEL_SEGMENT_LENGTH = 25;
static const float TUNNEL_START_Z = 2;
static const float TUNNEL_END_Z = -10;

// Constant values for single light source behind rocket
static const float lightPos[] =
	{ 0, 0, rocket.positionZ + 1.25 * TUNNEL_SEGMENT_LENGTH, 1 };
static const float lightAmb[] = { 0.35, 0.35, 0.35, 1 };
static const float lightDif[] = { 0.35, 0.35, 0.35, 1 };
static const float lightSpc[] = { 0.35, 0.35, 0.35, 1 };

// Constant values for fog color and density
static const float fogColor[] = { 0.1, 0.15, 0.2, 0.5 };
static const float fogDensity = 0.01;

// Score maintained globally
static int score;
static int scoreBonus;
static std::vector<int> highScores;

// Possible game states maintained globally
enum GameState { GAME_MENU, GAME_ACTIVE, GAME_PAUSED, GAME_OVER };
static GameState gameState = GAME_MENU;

// Draw tunnel as background
void drawTunnel()
{
	// Draw solid tunnel surfaces
	glEnable(GL_LIGHTING);
	setMaterial(tunnel);
	for (int i = 0; i < 360; i++)
	{
		glBegin(GL_QUADS);
			// Calculate normal, opposite from circumference line
			glNormal3f(cos((i + 0.5) * M_PI / 180) * -1,
				sin((i + 0.5) * M_PI / 180) * -1, 0);

			// Corner at front of tunnel view, no degree offset
			glVertex3f(
				TUNNEL_RADIUS * cos(i * M_PI / 180),
				TUNNEL_RADIUS * sin(i * M_PI / 180),
				rocket.positionZ
					+ TUNNEL_START_Z * TUNNEL_SEGMENT_LENGTH);

			// Corner at front of tunnel view, one degree off
			glVertex3f(
				TUNNEL_RADIUS * cos((i + 1) * M_PI / 180),
				TUNNEL_RADIUS * sin((i + 1) * M_PI / 180),
				rocket.positionZ
					+ TUNNEL_START_Z * TUNNEL_SEGMENT_LENGTH);

			// Corner at back of tunnel view, one degree off
			glVertex3f(
				TUNNEL_RADIUS * cos((i + 1) * M_PI / 180),
				TUNNEL_RADIUS * sin((i + 1) * M_PI / 180),
				rocket.positionZ
					+ TUNNEL_END_Z * TUNNEL_SEGMENT_LENGTH);

			// Corner at back of tunnel view, no degree offset
			glVertex3f(
				TUNNEL_RADIUS * cos(i * M_PI / 180),
				TUNNEL_RADIUS * sin(i * M_PI / 180),
				rocket.positionZ
					+ TUNNEL_END_Z * TUNNEL_SEGMENT_LENGTH);
		glEnd();
	}
	glDisable(GL_LIGHTING);

	// Draw lines along tunnel
	glColor3fv(tunnelLine);
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
		glColor3fv(tunnelLine);
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

// Helper function for object depth sorting
int closer(Object o1, Object o2)
{
	return o1.positionZ >= o2.positionZ;
}

// Position various objects within tunnel
void positionObjects()
{
	// Remove objects passed by rocket
	while (!objects.empty() && objects.at(0).positionZ
		> rocket.positionZ + TUNNEL_SEGMENT_LENGTH)
		objects.erase(objects.begin());

	// Continue generating objects randomly
	if (objects.empty() ||
		objects.back().positionZ - 1.25 * TUNNEL_SEGMENT_LENGTH >
			rocket.positionZ + TUNNEL_END_Z * TUNNEL_SEGMENT_LENGTH)
	{
		// Repeat for 1-3 iterations at each segment
		int iterations = rand() % 3 + 1;
		for (int i = 1; i <= iterations; i++)
		{
			// Generate coordinates to place object
			float x = (rand() % 3 - 1) * 1.6;
			float y = (rand() % 3 - 1) * 1.6;
			float z = rocket.positionZ
				+ (float)rand() / (float)RAND_MAX * TUNNEL_SEGMENT_LENGTH
				+ TUNNEL_END_Z * TUNNEL_SEGMENT_LENGTH;
			int type = rand() % 7 / 3 + 1;

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

	// Depth sorting for objects
	std::sort(objects.begin(), objects.end(), closer);
}

// Detect collisions with objects
void detectCollisions()
{
	// Test for collisions with all objects
	for (int i = 0; i < objects.size(); i++)
	{
		// Test for alignment in x, y, z directions
		int hitX = (int)round(rocket.positionX / 3)
			== (int)round(objects.at(i).positionX / 1.6);
		int hitY = (int)round(rocket.positionY / 3)
			== (int)round(objects.at(i).positionY / 1.6);
		int hitZ = fabs(objects.at(i).positionZ - rocket.positionZ
			- TUNNEL_SEGMENT_LENGTH + 2) <= 1.5;

		// Check for collision, don't handle event multiple times
		if (hitX && hitY && hitZ && !objects.at(i).collided)
		{
			// Increase score if picked up a coin
			if (objects.at(i).type == COIN)
			{
				scoreBonus += 100;
				scoreState = SCORE_INCREASE;
				scoreFlashTime = 100;
			}

			// Decrease score if hit an obstacle
			if (objects.at(i).type == OBSTACLE)
			{
				scoreBonus -= std::min(100, score);
				scoreState = SCORE_DECREASE;
				scoreFlashTime = 100;
			}

			// Game over if exploded by a bomb
			if (objects.at(i).type == BOMB)
			{
				// Ensure rocket is positioned at collided object
				rocket.positionX = objects.at(i).positionX * 3 / 1.6;
				rocket.positionY = objects.at(i).positionY * 3 / 1.6;

				// Set rocket as collided
				rocket.collided = 1;

				// Update game state
				gameState = GAME_OVER;

				// Update high scores list
				int index = 0;
				while (index < highScores.size())
				{
					if (score < highScores.at(index))
						index++;
					else
						break;
				}
				highScores.insert(highScores.begin() + index, score);
			}

			// Set flag that object has been collided with
			objects.at(i).collided = 1;
		}
	}
}

// Initialize or reset game
void newGame()
{
	// Remove objects
	objects.clear();

	// Reset rocket position
	rocket.positionX = 0;
	rocket.positionY = 0;
	rocket.positionZ = 0;

	// Reset rocket collision status
	rocket.collided = 0;

	// Reset rocket movement parameters
	oldRocketPosX = 0;
	oldRocketPosY = 0;
	newRocketPosX = 0;
	newRocketPosY = 0;
	percentRocketMoved = 100;
	burst = 0;

	// Reset score values
	score = 0;
	scoreBonus = 0;
	scoreFlashTime = 0;
	scoreState = SCORE_NORMAL;

	// Reset game over fade timer
	gameOverFadeTime = 0;

	// Reset Boolean flags
	showInstructions = 0;
	quitConfirmation = 0;
}

// Timer function for animation
void tick(int val)
{
	// Update timer if game over
	if (gameState == GAME_OVER)
		gameOverFadeTime = gameOverFadeTime + 1000 / 60;

	// Do nothing else if game not active
	if (gameState != GAME_ACTIVE)
		goto end;

	// Move rocket forward and accelerate until distance 25000
	rocket.positionZ -= std::min(0.5 - rocket.positionZ / 50000, 1.0)
		+ 0.5 * burst; // Faster speed during burst

	// Slide rocket in direction of x/y movement
	if (percentRocketMoved < 100)
	{
		rocket.positionX =
			(float)(oldRocketPosX * (100 - percentRocketMoved)
			+ newRocketPosX * percentRocketMoved) / 100.0f;
		rocket.positionY =
			(float)(oldRocketPosY * (100 - percentRocketMoved)
			+ newRocketPosY * percentRocketMoved) / 100.0f;
		percentRocketMoved += 20;
	}

	// Ensure rocket ends where expected
	else
	{
		oldRocketPosX = newRocketPosX;
		oldRocketPosY = newRocketPosY;
		rocket.positionX = newRocketPosX;
		rocket.positionY = newRocketPosY;
	}

	// Update score flash timer
	scoreFlashTime = std::max(0, scoreFlashTime - 20);

	// Perform specific actions for each object type
	for (int i = 0; i < objects.size(); i++)
	{
		// Rotate coins in place around y axis
		if (objects.at(i).type == COIN)
			objects.at(i).rotation += 3.5;

		// Accelerate obstacles toward rocket
		if (objects.at(i).type == OBSTACLE)
			objects.at(i).positionZ += 1;

		// Pulsating intensity for bombs
		if (objects.at(i).type == BOMB)
			objects.at(i).intensity = 1.5 + sin(rocket.positionZ) / 2;
	}

end:
	// Redraw continuously
	glutPostRedisplay();

	// Continuous animation at 60fps
	glutTimerFunc(1000 / 60, tick, 0);
}

// Display function: renders points on screen
void display()
{
	// Reset display before rendering scene
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// Color displayed at the end of tunnel
	glClearColor(0.08, 0.12, 0.16, 0);

	// Set perspective projection for world display
	setPerspectiveProjection(TUNNEL_SEGMENT_LENGTH
		* (TUNNEL_START_Z - TUNNEL_END_Z));

	// Position camera behind rocket
	gluLookAt(
		rocket.positionX / 2, rocket.positionY / 2,
		rocket.positionZ + TUNNEL_SEGMENT_LENGTH,
		0, 0, rocket.positionZ,
		0, 1, 0
	);

	// Tunnel in background
	glBindTexture(GL_TEXTURE_2D, 0);
	drawTunnel();

	// Calculate object positions
	positionObjects();

	// Detect collisions with all objects
	detectCollisions();

	// Render objects at calculated positions
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	for (int i = objects.size() - 1; i >= 0; i--)
	{
		// Coins with gold material, bind texture and display
		setMaterial(gold);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		if (objects.at(i).type == COIN)
			static_cast<Coin&>(objects.at(i)).render();

		// Rocks with chrome material, bind texture and display
		setMaterial(chrome);
		glBindTexture(GL_TEXTURE_2D, textures[1]);
		if (objects.at(i).type == OBSTACLE)
			static_cast<Obstacle&>(objects.at(i)).render();

		// Bombs with red matte material, bind texture and display
		setMaterial(redMatte);
		glBindTexture(GL_TEXTURE_2D, 0);
		if (objects.at(i).type == BOMB)
			static_cast<Bomb&>(objects.at(i)).render();
	}

	// Render rocket in foreground
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	rocket.render();

	// Disabling to clearly display texts
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_LIGHTING);

	// Display fire below rocket and score on fin when game is active
	if (gameState == GAME_ACTIVE)
	{
		// Calculate score (distance travelled plus bonus)
		score = (int)floor(-1 * rocket.positionZ) + scoreBonus;

		// Show score on rocket fin
		showScore(&rocket, score);

		// Enable rocket fire
		rocket.fireFunc();
	}

	// Set orthographic projection for menu and overlay
	setOrthographicProjection();

	// Call logic for main menu
	if (gameState == GAME_MENU)
		mainMenu(highScores);

	// Call logic for main menu
	if (gameState == GAME_PAUSED)
		pauseMenu();

	// Call logic for game over
	if (gameState == GAME_OVER)
		gameOver(score, highScores);

	// Flushes buffered commands to display
	glFlush();

	// Swap buffers for double buffering
	glutSwapBuffers();
}

// Keyboard function: handles standard keyboard controls
void keyboard(unsigned char key, int x, int y)
{
	// Perform action depending on character received
	switch (key)
	{
		// Pause active game
		case 'p':
		case 'P':
			if (gameState == GAME_ACTIVE)
			{
				gameState = GAME_PAUSED;
				quitConfirmation = 0;
			}
			break;

		// Quit game from main menu
		case 'q':
		case 'Q':
		case 27:
			if (gameState == GAME_MENU)
				exit(0);
			break;

		// Different behaviours depending on game state
		case ' ':
			switch (gameState)
			{
				// Toggle instructions or start game
				case GAME_MENU:
					if (glutGetModifiers() == GLUT_ACTIVE_SHIFT
						&& !showInstructions)
						showInstructions = 1;
					else if (showInstructions)
						showInstructions = 0;
					else
						gameState = GAME_ACTIVE;
					break;

				// Activate burst
				case GAME_ACTIVE:
					burst = 1;
					break;

				// Resume or quit paused game
				case GAME_PAUSED:
					if (quitConfirmation)
					{
						if (glutGetModifiers() == GLUT_ACTIVE_SHIFT)
						{
							gameState = GAME_MENU;
							newGame();
						}
						else
						{
							quitConfirmation = 0;
						}
					}
					else
					{
						if (glutGetModifiers() == GLUT_ACTIVE_SHIFT)
						{
							quitConfirmation = 1;
						}
						else
						{
							gameState = GAME_ACTIVE;
						}
					}
					break;

				// Return to menu after game over (or skip animation)
				case GAME_OVER:
					if (gameOverFadeTime < GAME_OVER_SHOW_MSG_TIME)
					{
						gameOverFadeTime = GAME_OVER_SHOW_MSG_TIME;
					}
					else
					{
						gameState = GAME_MENU;
						newGame();
					}
					break;
			}
			break;
	}
}

// Keyboard up function: handles key release on keyboard
void keyboardup(unsigned char key, int x, int y)
{
	switch (key)
	{
		// End burst
		case ' ':
			burst = 0;
			break;
	}
}

// Reshape function: adjust viewport on window load/resize
void reshape(int w, int h)
{
	DISPLAY_WIDTH = w;
	DISPLAY_HEIGHT = h;
	glViewport(0, 0, w, h);
}

// Special function: handle special keyboard controls
void special(int key, int x, int y)
{
	// Controls deactivated if game not active
	if (gameState != GAME_ACTIVE)
		return;

	// Skip transition effect in progress
	if (percentRocketMoved < 100)
	{
		oldRocketPosX = newRocketPosX;
		oldRocketPosY = newRocketPosY;
		rocket.positionX = newRocketPosX;
		rocket.positionY = newRocketPosY;
		percentRocketMoved = 100;
	}

	// Perform action depending on which key pressed
	switch (key)
	{
		// Arrow controls for rocket movement
		case GLUT_KEY_UP:
			oldRocketPosX = rocket.positionX;
			oldRocketPosY = rocket.positionY;
			newRocketPosX = rocket.positionX;
			newRocketPosY = oldRocketPosY == Y_DOWN ? Y_MIDDLE : Y_UP;
			percentRocketMoved = 0;
			break;
		case GLUT_KEY_DOWN:
			oldRocketPosX = rocket.positionX;
			oldRocketPosY = rocket.positionY;
			newRocketPosX = rocket.positionX;
			newRocketPosY = oldRocketPosY == Y_UP ? Y_MIDDLE : Y_DOWN;
			percentRocketMoved = 0;
			break;
		case GLUT_KEY_LEFT:
			oldRocketPosX = rocket.positionX;
			oldRocketPosY = rocket.positionY;
			newRocketPosX = oldRocketPosX == X_RIGHT ? X_MIDDLE : X_LEFT;
			newRocketPosY = rocket.positionY;
			percentRocketMoved = 0;
			break;
		case GLUT_KEY_RIGHT:
			oldRocketPosX = rocket.positionX;
			oldRocketPosY = rocket.positionY;
			newRocketPosX = oldRocketPosX == X_LEFT ? X_MIDDLE : X_RIGHT;
			newRocketPosY = rocket.positionY;
			percentRocketMoved = 0;
			break;
	}

	// Handle possible inaccuracies with rocket position
	oldRocketPosX = round(oldRocketPosX / 3) * 3;
	newRocketPosX = round(newRocketPosX / 3) * 3;
	oldRocketPosY = round(oldRocketPosY / 3) * 3;
	newRocketPosY = round(newRocketPosY / 3) * 3;
}

// Main function: entry point and initialization
int main(int argc, char ** argv)
{
	// Initial seed for randomization
	srand(time(NULL));

	// GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

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
	glutKeyboardUpFunc(keyboardup);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Disable key repeat
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enable fog
	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_EXP2);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, fogDensity);
	glHint(GL_FOG_HINT, GL_NICEST);

	// Enable normalization
	glEnable(GL_NORMALIZE);

	// Initialize lighting with constant values
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpc);
	glEnable(GL_LIGHT0);

	// Initialize textures
	initTextures();

	// Enable back-face culling
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	// Smooth shade model
	glShadeModel(GL_SMOOTH);

	// Initiate timer function for animation
	glutTimerFunc(1000 / 60, tick, 0);

	// Initialize game
	newGame();

	// Main program loop
	glutMainLoop();

	// Exit normally
	return 0;
}
