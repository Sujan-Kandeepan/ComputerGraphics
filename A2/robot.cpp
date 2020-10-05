// Standard C++ library imports
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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

// Enumerated type for menu/keyboard actions
enum Actions {
	MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT,
	TURN_LEFT, TURN_RIGHT,
	GRID10, GRID20, GRID30, GRID40, GRID50,
	RESET_POSITION, RANDOM_POSITION
};

// Enumerated type for robot direction
enum RobotDirection {
	ROBOT_UP, ROBOT_DOWN, ROBOT_LEFT, ROBOT_RIGHT
};

// Global float vector color values
float red[3] = { 1.0f, 0.0f, 0.0f };
float green[3] = { 0.0f, 1.0f, 0.0f };
float darkgrey[3] = { 0.25f, 0.25f, 0.25f };
float offwhite[3] = { 0.75f, 0.75f, 0.75f };

// Global constant for window size
const int windowSize = 600;

// Global constant for gridline thickness
const float gridlineThickness = 4;

// Global variable for grid size
int gridSize = 30;

// Global variables for robot position/direction
int robotSpawnX;
int robotX;
int robotSpawnY;
int robotY;
RobotDirection robotSpawnDirection;
RobotDirection robotDirection;

// Function to draw single point with given location and color
void drawPoint(float x, float y, float color[3])
{
	glColor3fv(color);
	glBegin(GL_POINTS);
		glVertex2f(x, y);
	glEnd();
}

// Function to draw horizontal line with given position and color
void drawHorLine(float xmin, float xmax, float y, float color[3])
{
	for (int x = xmin; x <= xmax; x++)
		drawPoint(x, y, color);
}

// Function to draw vertical line with given position and color
void drawVerLine(float x, float ymin, float ymax, float color[3])
{
	for (int y = ymin; y <= ymax; y++)
		drawPoint(x, y, color);
}

// Function to draw rectangle with given dimensions and color
void drawRectangle(float xmin, float xmax,
	float ymin, float ymax, float color[3])
{
	for (int y = ymin; y <= ymax; y++)
		for (int x = xmin; x <= xmax; x++)
			drawPoint(x, y, color);
}

// Function to draw individual cell within grid, optional border
void drawCell(float x, float y, float color[3], float border = 0)
{
	// Accept coordinates between 0 and grid size - 1
	assert(0 <= x && x < gridSize && 0 <= y && y < gridSize);

	// Draw rectangle within calculated bounds
	drawRectangle(
		x * (windowSize / gridSize) + floor(border),
		(x + 1) * (windowSize / gridSize) - (ceil(border) + 1),
		y * (windowSize / gridSize) + floor(border),
		(y + 1) * (windowSize / gridSize) - (ceil(border) + 1),
		color);
}

// Function to draw entire grid of lines
void drawGrid()
{
	for (int i = 0; i <= windowSize; i++)
	{
		if (i % (windowSize / gridSize) ==
			floor(gridlineThickness / 2))
				i += (windowSize / gridSize) - gridlineThickness;
		drawHorLine(0, windowSize, i, darkgrey);
		drawVerLine(i, 0, windowSize, darkgrey);
	}
}

// Function to draw all grid cells between lines
void drawCells()
{
	for (float y = 0; y < gridSize; y++)
		for (float x = 0; x < gridSize; x++)
			drawCell(x, y, offwhite, gridlineThickness / 2);
}

// Function to draw robot
void drawRobot()
{
	// Draw robot body at exact position
	drawCell(robotX, robotY, green);

	// Draw robot head at relative direction to body
	switch (robotDirection)
	{
		case ROBOT_UP:
			drawCell(robotX, robotY + 1, red);
			break;
		case ROBOT_DOWN:
			drawCell(robotX, robotY - 1, red);
			break;
		case ROBOT_LEFT:
			drawCell(robotX - 1, robotY, red);
			break;
		case ROBOT_RIGHT:
			drawCell(robotX + 1, robotY, red);
			break;
	}
}

// Function to keep robot within bounds on grid resize
void keepRobotWithinBounds(int *x, int *y,
	RobotDirection *d, int newGridSize)
{
	// Move left if already peering past right of new grid
	if (*d == ROBOT_RIGHT && *x % newGridSize == newGridSize - 1)
		*x = (*x - 1) % newGridSize;

	// Move right if already reversing past left of new grid
	else if (*d == ROBOT_LEFT && *x % newGridSize == 0)
		*x = (*x + 1) % newGridSize;

	// Resize with respect to current position otherwise
	else
		*x = *x % newGridSize;

	// Move down if already peering over top of new grid
	if (*d == ROBOT_UP && *y % newGridSize == newGridSize - 1)
		*y = (*y - 1) % newGridSize;

	// Move up if already reversing under bottom of new grid
	else if (*d == ROBOT_DOWN && *y % newGridSize == 0)
		*y = (*y + 1) % newGridSize;

	// Resize with respect to current position otherwise
	else
		*y = *y % newGridSize;
}

// Function to reset robot position
void resetRobotPosition()
{
	// Ensure respawn location is within bounds
	keepRobotWithinBounds(&robotSpawnX, &robotSpawnY,
		&robotSpawnDirection, gridSize);

	// Restore position and direction to previous
	robotX = robotSpawnX;
	robotY = robotSpawnY;
	robotDirection = robotSpawnDirection;
}

// Function to put robot at random position
void randomRobotPosition()
{
	// Generate random direction
	int mod4 = rand() % 4;
	if (mod4 % 4 == 0)
		robotSpawnDirection = ROBOT_UP;
	if (mod4 % 4 == 1)
		robotSpawnDirection = ROBOT_DOWN;
	if (mod4 % 4 == 2)
		robotSpawnDirection = ROBOT_LEFT;
	if (mod4 % 4 == 3)
		robotSpawnDirection = ROBOT_RIGHT;

	// Generate random location within bounds
	robotSpawnX = rand() % gridSize;
	robotSpawnY = rand() % gridSize;
	keepRobotWithinBounds(&robotSpawnX, &robotSpawnY,
		&robotSpawnDirection, gridSize);

	// Set position and direction to generated values
	robotX = robotSpawnX;
	robotY = robotSpawnY;
	robotDirection = robotSpawnDirection;
}

// Display function: renders grid and moving robot on screen
void display()
{
	// Clear OpenGL color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw grid with cells and place robot
	drawGrid();
	drawCells();
	drawRobot();

	// Flushes buffered commands to display
	glFlush();

	// Redraw grid and moving robot continuously
	glutPostRedisplay();
}

// Function to perform actions on menu tasks
void action(int value)
{
	// Perform action depending on enum value
	switch (value)
	{
		// Move robot up
		case MOVE_UP:
			if (robotY < gridSize -
				(robotDirection == ROBOT_UP ? 2 : 1))
				robotY++;
			break;

		// Move robot down
		case MOVE_DOWN:
			if (robotY > (robotDirection == ROBOT_DOWN ? 1 : 0))
				robotY--;
			break;

		// Move robot left
		case MOVE_LEFT:
			if (robotX > (robotDirection == ROBOT_LEFT ? 1 : 0))
				robotX--;
			break;

		// Move robot right
		case MOVE_RIGHT:
			if (robotX < gridSize -
				(robotDirection == ROBOT_RIGHT ? 2 : 1))
				robotX++;
			break;

		// Turn robot left if possible
		case TURN_LEFT:
			switch (robotDirection)
			{
				case ROBOT_UP:
					if (robotX > 0)
						robotDirection = ROBOT_LEFT;
					break;
				case ROBOT_DOWN:
					if (robotX < gridSize - 1)
						robotDirection = ROBOT_RIGHT;
					break;
				case ROBOT_LEFT:
					if (robotY > 0)
						robotDirection = ROBOT_DOWN;
					break;
				case ROBOT_RIGHT:
					if (robotY < gridSize - 1)
						robotDirection = ROBOT_UP;
					break;
			}
			break;

		// Turn robot right if possible
		case TURN_RIGHT:
			switch (robotDirection)
			{
				case ROBOT_UP:
					if (robotX < gridSize - 1)
						robotDirection = ROBOT_RIGHT;
					break;
				case ROBOT_DOWN:
					if (robotX > 0)
						robotDirection = ROBOT_LEFT;
					break;
				case ROBOT_LEFT:
					if (robotY < gridSize - 1)
						robotDirection = ROBOT_UP;
					break;
				case ROBOT_RIGHT:
					if (robotY > 0)
						robotDirection = ROBOT_DOWN;
					break;
			}
			break;

		// Set grid size to 10x10
		case GRID10:
			keepRobotWithinBounds(
				&robotX, &robotY, &robotDirection, 10);
			gridSize = 10;
			break;

		// Set grid size to 20x20
		case GRID20:
			keepRobotWithinBounds(
				&robotX, &robotY, &robotDirection, 20);
			gridSize = 20;
			break;

		// Set grid size to 30x30
		case GRID30:
			keepRobotWithinBounds(
				&robotX, &robotY, &robotDirection, 30);
			gridSize = 30;
			break;

		// Set grid size to 40x40
		case GRID40:
			keepRobotWithinBounds(
				&robotX, &robotY, &robotDirection, 40);
			gridSize = 40;
			break;

		// Set grid size to 50x50
		case GRID50:
			keepRobotWithinBounds(
				&robotX, &robotY, &robotDirection, 50);
			gridSize = 50;
			break;

		// Reset robot position
		case RESET_POSITION:
			resetRobotPosition();
			break;

		// Randomize robot position
		case RANDOM_POSITION:
			randomRobotPosition();
			break;
	}
}

// Function to generate left click menu
void createMenu()
{
	// Create submenu for robot movement
	int moveMenu = glutCreateMenu(action);
	glutAddMenuEntry("Move up", MOVE_UP);
	glutAddMenuEntry("Move down", MOVE_DOWN);
	glutAddMenuEntry("Move left", MOVE_LEFT);
	glutAddMenuEntry("Move right", MOVE_RIGHT);
	glutAttachMenu(GLUT_LEFT_BUTTON);

	// Create submenu robot turning
	int turnMenu = glutCreateMenu(action);
	glutAddMenuEntry("Turn left", TURN_LEFT);
	glutAddMenuEntry("Turn right", TURN_RIGHT);
	glutAttachMenu(GLUT_LEFT_BUTTON);

	// Create submenu for grid size selection
	int sizeMenu = glutCreateMenu(action);
	glutAddMenuEntry("10x10 grid", GRID10);
	glutAddMenuEntry("20x20 grid", GRID20);
	glutAddMenuEntry("30x30 grid", GRID30);
	glutAddMenuEntry("40x40 grid", GRID40);
	glutAddMenuEntry("50x50 grid", GRID50);
	glutAttachMenu(GLUT_LEFT_BUTTON);

	// Create top-level menu
	glutCreateMenu(action);
	glutAddSubMenu("Move Robot", moveMenu);
	glutAddSubMenu("Turn Robot", turnMenu);
	glutAddSubMenu("Grid Size", sizeMenu);
	glutAddMenuEntry("Reset Position", RESET_POSITION);
	glutAddMenuEntry("Random Position", RANDOM_POSITION);
	glutAttachMenu(GLUT_LEFT_BUTTON);
}

// Keyboard function: handles standard keyboard controls
void keyboard(unsigned char key, int x, int y)
{
	// Perform action depending on character received
	switch (key)
	{
		// Change grid size
		case '1':
			action(GRID10);
			break;
		case '2':
			action(GRID20);
			break;
		case '3':
			action(GRID30);
			break;
		case '4':
			action(GRID40);
			break;
		case '5':
			action(GRID50);
			break;

		// Exit program
		case 'q':
		case 'Q':
		case 27:
			exit(0);
			break;
	}
}

// Special function: handle special keyboard controls
void special(int key, int x, int y)
{
	// Perform action depending on which key pressed
	switch (key)
	{
		// Arrow controls for robot movement
		case GLUT_KEY_UP:
			action(MOVE_UP);
			break;
		case GLUT_KEY_DOWN:
			action(MOVE_DOWN);
			break;
		case GLUT_KEY_LEFT:
			action(MOVE_LEFT);
			break;
		case GLUT_KEY_RIGHT:
			action(MOVE_RIGHT);
			break;

		// Page up/down to turn robot left/right
		case GLUT_KEY_PAGE_UP:
			action(TURN_LEFT);
			break;
		case GLUT_KEY_PAGE_DOWN:
			action(TURN_RIGHT);
			break;

		// Reset robot position
		case GLUT_KEY_HOME:
			action(RESET_POSITION);
			break;

		// Randomize robot position
		case GLUT_KEY_END:
			action(RANDOM_POSITION);
			break;
	}
}

// Main function: entry point and initialization
int main(int argc, char ** argv)
{
	// GLUT initialization
	glutInit(&argc, argv);

	// Set window dimensions to global square window size
	glutInitWindowSize(windowSize, windowSize);
	glutInitWindowPosition(
		(glutGet(GLUT_SCREEN_WIDTH) - windowSize) / 2,
		(glutGet(GLUT_SCREEN_HEIGHT) - windowSize) / 2);

	// Create window for displaying grid and moving robot
	glutCreateWindow("3GC3 - Assignment 2 (Part A)");

	// Set coordinate system by global square window size
	gluOrtho2D(0, windowSize, 0, windowSize);

	// Set up left click menu
	createMenu();

	// Position robot randomly
	randomRobotPosition();

	// I/O function bindings
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);

	// Main program loop
	glutMainLoop();

	// Exit normally
	return 0;
}
