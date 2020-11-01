// Standard C++ library imports
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

// Values for window and grid size initialization
static const int minGridSize = 50;
static const int maxGridSize = 300;
static const int windowSize = 600;
static int secondDisplayWidth;
static int secondDisplayHeight;

// RGB float values for terrain colors
static float lowColor[] = { 0.150, 0.150, 0.000 };
static float highColor[] = { 0.000, 0.500, 0.000 };
static float wireColor[] = { 0.750, 0.750, 0.750 };

// User-selected grid size
static int gridSize = 0;

// Height map for points on terrain
float heightMap[maxGridSize][maxGridSize];

// Normal map for squares, triangles pointing to x and z axes, and vertices
float squareNormals[maxGridSize][maxGridSize][3];
float triangleXNormals[maxGridSize][maxGridSize][3];
float triangleZNormals[maxGridSize][maxGridSize][3];
float vertexNormals[maxGridSize][maxGridSize][3];
enum NormalType { SQUARE, TRIANGLEX, TRIANGLEZ, VERTEX };

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

// Enum definition and global variable for lighting mode
enum LightingMode { SOURCE1, SOURCE2, OFF };
LightingMode lightingMode = SOURCE1;

// Enum definition and global variable for lighting mode
enum ShadingMode { FLAT, GOURAUD };
ShadingMode shadingMode = FLAT;

// Calculate polygon face normal given 3 x/z coordinates
void calculateFaceNormal(int x1, int z1, int x2, int z2,
	int x3, int z3, NormalType n)
{
	// Vector a from point 2 to point 3
	static float a[] =
	{
		x3 - x2,
		heightMap[x3][z3] - heightMap[x2][z2],
		z3 - z2
	};

	// Vector b from point 2 to point 1
	static float b[] =
	{
		x1 - x2,
		heightMap[x1][z1] - heightMap[x2][z2],
		z1 - z2
	};

	// Vector c = a x b
	static float c[] =
	{
		a[1] * b[2] - a[2] * b[1],
		a[2] * b[0] - a[0] * b[2],
		a[0] * b[1] - a[1] * b[0]
	};

	// Assign to respective normal map
	if (n == SQUARE)
		for (int i = 0; i < 3; i++)
			squareNormals[x1][z1][i] = c[i];
	if (n == TRIANGLEX)
		for (int i = 0; i < 3; i++)
			triangleXNormals[x1][z1][i] = c[i];
	if (n == TRIANGLEZ)
		for (int i = 0; i < 3; i++)
			triangleZNormals[x1][z1][i] = c[i];
}

// Precompute normals depending on polygons and shading
void precomputeNormals()
{
	// Compute all face normals first
	for (int x = 0; x < gridSize - 1; x++)
	{
		for (int z = 0; z < gridSize - 1; z++)
		{
			calculateFaceNormal(x, z, x, z + 1, x + 1, z + 1, SQUARE);
			calculateFaceNormal(x, z, x + 1, z + 1, x + 1, z, TRIANGLEX);
			calculateFaceNormal(x, z, x, z + 1, x + 1, z + 1, TRIANGLEZ);
		}
	}

	// Skip vertex normal computation for flat shading
	if (shadingMode == FLAT)
		return;

	// Compute vertex normals, assume upward face normals if outside grid
	for (int x = 0; x < gridSize; x++)
	{
		for (int z = 0; z < gridSize; z++)
		{
			// Accumulate normal, will be normalized by GL_NORMALIZE
			float normal[3] = { 0, 0, 0 };

			// Different faces for squares
			if (stripsMode == SQUARES)
			{
				// Square face at (x - 1, z - 1)
				if (x >= 1 && z >= 1)
					for (int i = 0; i < 3; i++)
						normal[i] += squareNormals[x - 1][z - 1][i];
				else
					normal[1] += 1;

				// Square face at (x, z - 1)
				if (x < gridSize - 1 && z >= 1)
					for (int i = 0; i < 3; i++)
						normal[i] += squareNormals[x][z - 1][i];
				else
					normal[1] += 1;

				// Square face at (x - 1, z)
				if (x >= 1 && z < gridSize - 1)
					for (int i = 0; i < 3; i++)
						normal[i] += squareNormals[x - 1][z][i];
				else
					normal[1] += 1;

				// Square face at (x, z)
				if (x < gridSize - 1 && z < gridSize - 1)
					for (int i = 0; i < 3; i++)
						normal[i] += squareNormals[x][z][i];
				else
					normal[1] += 1;
			}

			//  Different faces for triangles
			if (stripsMode == TRIANGLES)
			{
				// Triangle pointing to x axis at (x - 1, z - 1)
				if (x >= 1 && z >= 1)
					for (int i = 0; i < 3; i++)
						normal[i] += triangleXNormals[x - 1][z - 1][i];
				else
					normal[1] += 1;

				// Triangle pointing to z axis at (x - 1, z - 1)
				if (x >= 1 && z >= 1)
					for (int i = 0; i < 3; i++)
						normal[i] += triangleZNormals[x - 1][z - 1][i];
				else
					normal[1] += 1;

				// Triangle pointing to z axis at (x, z - 1)
				if (x < gridSize - 1 && z >= 1)
					for (int i = 0; i < 3; i++)
						normal[i] += triangleZNormals[x][z - 1][i];
				else
					normal[1] += 1;

				// Triangle pointing to x axis at (x - 1, z)
				if (x >= 1 && z < gridSize - 1)
					for (int i = 0; i < 3; i++)
						normal[i] += triangleXNormals[x - 1][z][i];
				else
					normal[1] += 1;

				// Triangle pointing to x axis at (x, z)
				if (x < gridSize - 1 && z < gridSize - 1)
					for (int i = 0; i < 3; i++)
						normal[i] += triangleXNormals[x][z][i];
				else
					normal[1] += 1;

				// Triangle pointing to z axis at (x, z)
				if (x < gridSize - 1 && z < gridSize - 1)
					for (int i = 0; i < 3; i++)
						normal[i] += triangleZNormals[x][z][i];
				else
					normal[1] += 1;
			}

			// Set value in vertex normal map
			for (int i = 0; i < 3; i++)
				vertexNormals[x][z][i] = normal[i];
		}
	}
}

// Draw a vertex at a given location
void drawVertex(int x, int z, int wireframe, int window)
{
	// Compute height normalized wrt. highest point
	float height = heightMap[x][z] / greatestHeight;

	// Set color for wireframe
	if (wireframe && window == 1)
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

	// Set vertex normal to precomputed value for Gouraud shading
	if (shadingMode == GOURAUD)
		glNormal3fv(vertexNormals[x][z]);

	// Draw vertex for first window (slightly higher for wireframe)
	if (window == 1)
		glVertex3f(
			0 - (gridSize - 1) / 2 + x,
			heightMap[x][z] + (wireframe ? 0.1 : 0),
			0 - (gridSize - 1) / 2 + z);

	// Draw vertex for second window
	if (window == 2)
		glVertex2f(x * secondDisplayWidth / (gridSize - 1),
			(gridSize - 1 - z) * secondDisplayWidth / (gridSize - 1));
}

// Draw 2x2 sub-grid adjacent to vertex using square
void drawSquareCell(int x, int z, int wireframe, int window)
{
	// Set to either wireframe or solid shape
	if (wireframe && window == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Draw square with calculated normals
	glBegin(GL_QUADS);
		if (shadingMode == FLAT)
			glNormal3fv(squareNormals[x][z]);
		drawVertex(x, z, wireframe, window);
		drawVertex(x, z + 1, wireframe, window);
		drawVertex(x + 1, z + 1, wireframe, window);
		drawVertex(x + 1, z, wireframe, window);
	glEnd();
}

// Draw 2x2 sub-grid adjacent to vertex using two triangles
void drawTriangleCell(int x, int z, int wireframe, int window)
{
	// Set to either wireframe or solid shape
	if (wireframe && window == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Draw first of two connected triangles with calculated normals
	glBegin(GL_TRIANGLES);
		if (shadingMode == FLAT)
			glNormal3fv(triangleXNormals[x][z]);
		drawVertex(x, z, wireframe, window);
		drawVertex(x + 1, z + 1, wireframe, window);
		drawVertex(x + 1, z, wireframe, window);
	glEnd();

	// Draw second of two connected triangles with calculated normals
	glBegin(GL_TRIANGLES);
		if (shadingMode == FLAT)
			glNormal3fv(triangleZNormals[x][z]);
		drawVertex(x, z, wireframe, window);
		drawVertex(x, z + 1, wireframe, window);
		drawVertex(x + 1, z + 1, wireframe, window);
	glEnd();
}

// Draw terrain on first and second window
void drawTerrain(int window)
{
	precomputeNormals();

	// Iterate over all points first time
	for (int x = 0; x < gridSize - 1; x++)
	{
		for (int z = 0; z < gridSize - 1; z++)
		{
			// Draw solid shapes if applicable
			if (wireframeMode != WIREFRAME)
			{
				if (stripsMode == TRIANGLES)
					drawTriangleCell(x, z, 0, window);
				else
					drawSquareCell(x, z, 0, window);
			}
		}
	}

	// Iterate over all points second time
	for (int x = 0; x < gridSize - 1; x++)
	{
		for (int z = 0; z < gridSize - 1; z++)
		{
			// Draw wireframes if applicable
			if (wireframeMode != SOLID)
			{
				if (stripsMode == TRIANGLES)
					drawTriangleCell(x, z, 1, window);
				else
					drawSquareCell(x, z, 1, window);
			}
		}
	}

	// Flushes buffered commands to display
	glFlush();

	// Redraw terrain continuously
	glutPostRedisplay();
}

// Add circle function: add circle in circles algorithm
void addCircle(float circleX, float circleZ,
	float circleSize, float circleDisplacement)
{
	// Reset stored value for greatest height
	greatestHeight = 0;

	// Adjust height for every point in grid
	for (int x = 0; x < gridSize; x++)
	{
		for (int z = 0; z < gridSize; z++)
		{
			// Compute normalized distance from circle center
			float distance = sqrt(pow(circleX - x, 2)
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

// Generate new terrain using circles algorithm
void newTerrain()
{
	// Variable declarations for circle dimensions/placement
	float circleX, circleZ, circleSize, circleDisplacement;

	// Initialize all height values to 0
	for (int x = 0; x < gridSize; x++)
		for (int z = 0; z < gridSize; z++)
			heightMap[x][z] = 0;

	// Iterate number of times given by grid size
	for (int i = 0; i < gridSize; i++)
	{
		// Randomize circle dimensions/placement
		circleX = rand() % gridSize;
		circleZ = rand() % gridSize;
		circleSize = rand() % gridSize / 2 + 5;
		circleDisplacement = circleSize / gridSize * 5;

		// Add circle pushing up using common logic
		addCircle(circleX, circleZ, circleSize, circleDisplacement);
	}
}

// Display function: renders terrain on first window
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

	// Set up first light source (top near left)
	float pos0[] = { -1 * gridSize, gridSize, gridSize, 0.5 };
	float amb0[] = { 1, 1, 1, 0.5 };
	float dif0[] = { 0.5, 0.5, 0.5, 0.5 };
	float spc0[] = { 1, 1, 1, 0.5 };
	glLightfv(GL_LIGHT0, GL_POSITION, pos0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dif0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spc0);

	// Set up second light source (top far right)
	float pos1[] = { gridSize, gridSize, -1 * gridSize, 0.5 };
	float amb1[] = { 1, 1, 1, 0.5 };
	float dif1[] = { 0.5, 0.5, 0.5, 0.5 };
	float spc1[] = { 1, 1, 1, 0.5 };
	glLightfv(GL_LIGHT1, GL_POSITION, pos1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, amb1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, dif1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, spc1);

	// Enable lighting at first light source
	if (lightingMode == SOURCE1)
	{
		glEnable(GL_LIGHTING);
		glDisable(GL_LIGHT1);
		glEnable(GL_LIGHT0);
	}

	// Enable lighting at second light source
	if (lightingMode == SOURCE2)
	{
		glEnable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
	}

	// Disable lighting temporarily
	if (lightingMode == OFF)
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
	}

	// Set to flat shading if selected
	if (shadingMode == FLAT)
		glShadeModel(GL_FLAT);


	// Set to Gouraud shading if selected
	if (shadingMode == GOURAUD)
		glShadeModel(GL_SMOOTH);

	// Rotate before displaying terrain
	glRotatef(cameraRotationX, 1, 0, 0);
	glRotatef(cameraRotationY, 0, 1, 0);

	// Draw terrain using common logic
	drawTerrain(1);
}

// Display 2 function: renders terrain overview on second window
void display2()
{
	// Clear OpenGL color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw terrain using common logic
	drawTerrain(2);
}

// Keyboard function: handles standard keyboard controls
void keyboard(unsigned char key, int x, int y)
{
	// Perform action depending on character received
	switch (key)
	{
		// Toggle shading
		case 'g':
		case 'G':
			shadingMode = (ShadingMode)(!shadingMode);
			break;

		// Toggle lighting
		case 'l':
		case 'L':
			lightingMode = (LightingMode)((lightingMode + 1) % 3);
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

// Mouse function: terrain adjustment using mouse click on second window
void mouse(int button, int state, int x, int y)
{
	// Scale and round coordinates based on grid size
	x = floor(x * gridSize / secondDisplayWidth);
	y = floor(y * gridSize / secondDisplayHeight);

	// Increase height within limit on left mouse click
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		// Add circle pushing upward using common logic
		if (heightMap[x][y] <= gridSize / 3)
			addCircle(x, y, gridSize / 5, gridSize / 50);
	}

	// Decrease height within limit on right mouse click
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		// Add circle pushing downward using common logic
		if (heightMap[x][y] >= 1)
			addCircle(x, y, gridSize / 5, -1 * gridSize / 50);
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

// Reshape function: adjusts terrain view upon resize of first window
void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, maxGridSize * 2);
	glMatrixMode(GL_MODELVIEW);
}

// Reshape 2 function: adjusts terrain overviw upon resize of second window
void reshape2(int w, int h)
{
	// Update global constants for window dimensions
	secondDisplayWidth = w;
	secondDisplayHeight = h;

	// Reset pixel matrix with new dimensions
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, secondDisplayWidth, 0, secondDisplayHeight);

	// Reconstruct view of 2D projection
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, secondDisplayWidth, secondDisplayHeight);
}

// Main function: entry point and initialization
int main(int argc, char ** argv)
{
	// Prompt for square grid size in command line
	while (gridSize < minGridSize || gridSize > maxGridSize)
	{
		printf("Enter a grid size for the terrain"
			" (between 50 and 300): ");
		scanf("%d", &gridSize);
	}

	// Display keyboard controls to command line
	printf("Keyboard controls (both windows):\n"
		" - Arrows -> Move camera\n"
		" - G      -> Toggle shading (flat or Gouraud)\n"
		" - L      -> Toggle lighting (2 sources or off)\n"
		" - Q/Esc  -> Quit program\n"
		" - R      -> Reset terrain\n"
		" - S      -> Toggle strips (polygon) mode\n"
		" - W      -> Toggle wireframe mode\n"
		"Mouse controls (second window only):\n"
		" - Left click  -> Increase height where clicked\n"
		" - Right click -> Decrease height where clicked\n");

	// Generate new terrain
	newTerrain();

	// GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);

	// Set dimensions and position of first window
	glutInitWindowSize(windowSize, windowSize);
	glutInitWindowPosition(
		(glutGet(GLUT_SCREEN_WIDTH) - windowSize) / 4,
		(glutGet(GLUT_SCREEN_HEIGHT) - windowSize) / 2);

	// Create window for displaying terrain
	glutCreateWindow("3GC3 - Assignment 3 | 3D Camera View");

	// I/O function bindings
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);

	// Enable backface culling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Enable lighting with color
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);

	// Set dimensions and position of second window
	glutInitWindowSize(windowSize, windowSize);
	glutInitWindowPosition(
		(glutGet(GLUT_SCREEN_WIDTH) - windowSize) * 3 / 4,
		(glutGet(GLUT_SCREEN_HEIGHT) - windowSize) / 2);

	// Create window for displaying terrain
	glutCreateWindow("3GC3 - Assignment 3 | 2D Overview");

	// I/O function bindings
	glutDisplayFunc(display2);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutReshapeFunc(reshape2);
	glutSpecialFunc(special);

	// Main program loop
	glutMainLoop();

	// Exit normally
	return 0;
}