// Standard C++ library imports
#include <stdio.h>
#include <stdlib.h>
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

// Struct holding RGB values for a color
struct color {
	float value[3];
	color(){}
	color(float red, float green, float blue)
	{
		value[0] = red;
		value[1] = green;
		value[2] = blue;
	}
};

// Struct holding (x, y) coordinates, color and size of a point
struct point {
	float pointX;
	float pointY;
	color pointColor;
	float pointSize;
	point(){}
};

// Static array of 5 brush colors available
static color availableBackgroundColors[5] = {
	color(0.100, 0.100, 0.100), // light black
	color(0.545, 0.000, 0.000), // dark red
	color(0.545, 0.271, 0.075), // saddle brown
	color(0.000, 0.392, 0.000), // medium spring green
	color(0.098, 0.098, 0.439)  // deep sky blue
};

// Static array of 5 brush colors available
static color availablePointColors[5] = {
	color(0.961, 0.961, 0.961), // white smoke
	color(1.000, 0.271, 0.000), // orange red
	color(1.000, 0.843, 0.000), // gold
	color(0.000, 1.000, 0.498), // medium spring green
	color(0.000, 0.749, 1.000)  // deep sky blue
};

// Static value of current background color
static color currentBackgroundColor = availableBackgroundColors[0];

// Static value of current brush color
static color currentPointColor = availablePointColors[0];

// Static value of current point size (1-5)
static float currentPointSize = 3;

// Static global array of points on screen
static std::vector<point> drawnPoints;

// Static global values for display dimensions
static int DISPLAY_WIDTH = 500;
static int DISPLAY_HEIGHT = 500;
static int DISPLAY_OFFSET_WIDTH;
static int DISPLAY_OFFSET_HEIGHT;

// Display function: renders points on screen
void display()
{
	// Clear OpenGL color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Set background color before drawing points
	glClearColor(currentBackgroundColor.value[0],
		currentBackgroundColor.value[1],
		currentBackgroundColor.value[2], 0);

	// Iterate over all stored points to redraw
	for (int i = 0; i < drawnPoints.size(); i++)
	{
		// Draw point with stored size, color and (x, y) location
		glPointSize(drawnPoints.at(i).pointSize);
		glBegin(GL_POINTS);
		glColor3fv(drawnPoints.at(i).pointColor.value);
		glVertex2f(drawnPoints.at(i).pointX,
			drawnPoints.at(i).pointY);
		glEnd();
	}

	// Flushes buffered commands to display
	glFlush();

	// Redraw points continuously
	glutPostRedisplay();
}

// Keyboard function: handles keyboard controls
void keyboard(unsigned char key, int x, int y)
{
	// Perform action depending on character received
	switch (key)
	{
		// Change brush color
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
			currentPointColor =
				availablePointColors[key - '1'];
			break;
		// Change background colors
		case '6':
		case '7':
		case '8':
		case '9':
		case '0':
			currentBackgroundColor =
				availableBackgroundColors[key - '6'
					+ (key == '0' ? 10 : 0)];
			break;
		// Increment point size (highest is 5)
		case '+':
			if (currentPointSize < 5)
				currentPointSize++;
			else
				currentPointSize = 5;
			break;
		// Decrement point size (lowest is 1)
		case '-':
			if (currentPointSize > 1)
				currentPointSize--;
			else
				currentPointSize = 1;
			break;
		// Exit program
		case 'q':
		case 'Q':
			exit(0);
			break;
		// Clear window
		case 'r':
		case 'R':
			drawnPoints.clear();
			break;
	}
}

// Mouse function: registers points during mouse click
void mouse(int button, int state, int x, int y)
{
	// Adjust inverted y coordinate 
	y = DISPLAY_HEIGHT - y;

	// Create and store point on left mouse click
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		// Use (x, y) location of mouse, current point color/size
		point tempPoint;
		tempPoint.pointX = (float)x;
		tempPoint.pointY = (float)y;
		tempPoint.pointColor = currentPointColor;
		tempPoint.pointSize = currentPointSize;
		drawnPoints.push_back(tempPoint);
	}
}

// Motion function: creates several points during mouse drag
void motion(int x, int y)
{
	// Re-use logic in mouse function
	mouse(GLUT_LEFT_BUTTON, GLUT_DOWN, x, y);
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

	// Create window (main drawing canvas)
	glutCreateWindow("3GC3 - Assignment 1");

	// Make points into circles
	glEnable(GL_POINT_SMOOTH);

	// Set coordinate system to match window dimensions
	gluOrtho2D(0, DISPLAY_WIDTH, 0, DISPLAY_HEIGHT);

	// I/O function bindings
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	// Main program loop
	glutMainLoop();

	// Exit normally
	return 0;
}