// Standard C++ library imports
#include <math.h>
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

// Constants for window and grid size initialization
static const int minGridSize = 50;
static const int maxGridSize = 300;
static const int windowSize = 600;

// RGB float values for terrain colors
static float lowColor[] = { 0.150, 0.150, 0.000 };
static float highColor[] = { 0.000, 0.500, 0.000 };
static float wireColor[] = { 0.750, 0.750, 0.750 };

// User-selected grid size
static int gridSize = 0;

// Height map for points on terrain
float heightMap[maxGridSize][maxGridSize];

// Global value for tallest height in current terrain
static float greatestHeight;

// Position of camera rotation around x/y axis
static float cameraRotationX = 15, cameraRotationY = 0;

// Enum definition and global variable for wireframe mode
enum WireframeMode { SOLID, WIREFRAME, BOTH };
WireframeMode wireframeMode = SOLID;

// Enum definition and global variable for strips (polygon) mode
enum StripsMode { SQUARES, TRIANGLES };
StripsMode stripsMode = SQUARES;

// Draw a vertex at a given location
void drawVertex(int x, int z, int wireframe)
{
	// Compute height normalized wrt. highest point
	float height = heightMap[x][z] / greatestHeight;

	// Set color for wireframe
	if (wireframe)
		glColor3fv(wireColor);

	// Set color for solid shape
	else
		glColor3f(
			(lowColor[0] * (1 - height)
				+ highColor[0] * (height)),
			(lowColor[1] * (1 - height)
				+ highColor[1] * (height)),
			(lowColor[2] * (1 - height)
				+ highColor[2] * (height)));

	// Draw vertex (slightly higher for wireframe)
	glVertex3f(
		0 - (gridSize - 1) / 2 + x,
		heightMap[x][z] + (wireframe ? 0.1 : 0),
		0 - (gridSize - 1) / 2 + z);
}

// Draw 2x2 sub-grid adjacent to vertex using square
void drawSquareCell(int x, int z, int wireframe)
{
	// Set to either wireframe or solid shape
	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Draw square counter-clockwise
	glBegin(GL_QUADS);
		drawVertex(x, z, wireframe);
		drawVertex(x, z + 1, wireframe);
		drawVertex(x + 1, z + 1, wireframe);
		drawVertex(x + 1, z, wireframe);
	glEnd();
}

// Draw 2x2 sub-grid adjacent to vertex using two triangles
void drawTriangleCell(int x, int z, int wireframe)
{
	// Set to either wireframe or solid shape
	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Draw two connected triangles counter-clockwise
	glBegin(GL_TRIANGLES);
		drawVertex(x, z, wireframe);
		drawVertex(x, z + 1, wireframe);
		drawVertex(x + 1, z + 1, wireframe);
		drawVertex(x, z, wireframe);
		drawVertex(x + 1, z + 1, wireframe);
		drawVertex(x + 1, z, wireframe);
	glEnd();
}

// New terrain function: circles algorithm for new terrain
void newTerrain()
{
	// Variable declarations for circle dimensions/placement
	float circleX, circleZ, circleSize,
		circleDisplacement, distance;

	// Initialize all height values to 0
	for (int x = 0; x < gridSize; x++)
		for (int z = 0; z < gridSize; z++)
			heightMap[x][z] = 0;

	// Iterate number of times given by grid size
	for (int i = 0; i < gridSize; i++) {
		// Randomize circle dimensions/placement
		circleX = rand() % gridSize;
		circleZ = rand() % gridSize;
		circleSize = rand() % gridSize / 2 + 5;
		circleDisplacement = 1;

		// Adjust height for every point in grid
		for (int x = 0; x < gridSize; x++) {
			for (int z = 0; z < gridSize; z++) {
				// Compute normalized distance from circle center
				distance = sqrt(pow(circleX - x, 2)
					+ pow(circleZ - z, 2));
				distance = distance * 2 / circleSize;

				// Adjust height using cosine function
				if (fabs(distance) <= 1)
					heightMap[x][z] += circleDisplacement/2
						* (cos(distance * M_PI) + 1);

				// Keep record of greatest height
				if (heightMap[x][z] > greatestHeight)
					greatestHeight = heightMap[x][z];
			}
		}
	}
}

// Display function: renders terrain on screen
void display()
{
	// Reset display before rendering terrain
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// Position camera in front of terrain
	gluLookAt(
		0, 0, gridSize,
		0, 0, 0,
		0, 1, 0
	);

	// Rotate before displaying terrain
	glRotatef(cameraRotationX, 1, 0, 0);
	glRotatef(cameraRotationY, 0, 1, 0);

	// Iterate over all points first time
	for (int x = 0; x < gridSize - 1; x++)
	{
		for (int z = 0; z < gridSize - 1; z++)
		{
			// Draw solid shapes if applicable
			if (wireframeMode != WIREFRAME) {
				if (stripsMode == TRIANGLES)
					drawTriangleCell(x, z, 0);
				else
					drawSquareCell(x, z, 0);
			}
		}
	}

	// Iterate over all points second time
	for (int x = 0; x < gridSize - 1; x++)
	{
		for (int z = 0; z < gridSize - 1; z++)
		{
			// Draw wireframes if applicable
			if (wireframeMode != SOLID) {
				if (stripsMode == TRIANGLES)
					drawTriangleCell(x, z, 1);
				else
					drawSquareCell(x, z, 1);
			}
		}
	}

	// Flushes buffered commands to display
	glFlush();

	// Redraw terrain continuously
	glutPostRedisplay();
}

// Keyboard function: handles standard keyboard controls
void keyboard(unsigned char key, int x, int y)
{
	// Perform action depending on character received
	switch (key)
	{
		// Toggle lighting
		case 'l':
		case 'L':
			// TODO: implement lighting
			break;

		// Quit program
		case 'q':
		case 'Q':
		case 27:
			exit(0);
			break;

		// Reset terrain
		case 'r':
		case 'R':
			newTerrain();
			break;

		// Toggle strips (polygon) mode
		case 's':
		case 'S':
			stripsMode = (StripsMode)(!stripsMode);
			break;

		// Toggle wireframe mode
		case 'w':
		case 'W':
			wireframeMode =
				(WireframeMode)((wireframeMode + 1) % 3);
			break;
	}
}

// Special function: handle special keyboard controls
void special(int key, int x, int y)
{
	// Perform action depending on which key pressed
	switch (key)
	{
		// Pan camera up until directly above
		case GLUT_KEY_UP:
			if (cameraRotationX < 90)
				cameraRotationX++;
			break;

		// Pan camera down until almost level
		case GLUT_KEY_DOWN:
			if (cameraRotationX > 5)
				cameraRotationX--;
			break;

		// Rotate camera clockwise around terrain
		case GLUT_KEY_LEFT:
			cameraRotationY++;
			break;

		// Rotate camera counter-clockwise around terrain
		case GLUT_KEY_RIGHT:
			cameraRotationY--;
			break;
	}
}

// Reshape function: adjusts terrain view upon window resize
void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, maxGridSize * 2);
	glMatrixMode(GL_MODELVIEW);
}

// Main function: entry point and initialization
int main(int argc, char ** argv)
{
	// Prompt for square grid size in command line
	while (gridSize < minGridSize || gridSize > maxGridSize) {
		printf("Enter a grid size for the terrain"
			" (between 50 and 300): ");
		scanf("%d", &gridSize);
	}

	// Display keyboard controls to command line
	printf("Keyboard controls:\n"
		"Arrows - Move camera\n"
		"L      - Toggle lighting\n"
		"Q/Esc  - Quit program\n"
		"R      - Reset terrain\n"
		"S      - Toggle strips (polygon) mode\n"
		"W      - Toggle wireframe mode\n");

	// Generate new terrain
	newTerrain();

	// GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);

	// Set window dimensions to constant size
	glutInitWindowSize(windowSize, windowSize);
	glutInitWindowPosition(
		(glutGet(GLUT_SCREEN_WIDTH) - windowSize) / 2,
		(glutGet(GLUT_SCREEN_HEIGHT) - windowSize) / 2);

	// Create window for displaying grid and moving robot
	glutCreateWindow("3GC3 - Assignment 3");

	// I/O function bindings
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);

	// Enable backface culling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Main program loop
	glutMainLoop();

	// Exit normally
	return 0;
}