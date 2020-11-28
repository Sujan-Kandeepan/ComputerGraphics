// Standard C++ library imports
#include <float.h>
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

// Local library imports
#include "material.cpp"
#include "object.cpp"

// Enum for direction (used for keyboard actions)
enum Direction { NONE, LEFT, RIGHT, DOWN, UP, BACK, FRONT };
Direction direction = NONE;

// Linear scene graph of objects
std::vector<Object> objects;

// Index of selected object
int selected = 0;

// Constant values for display
const float axisLength = 100;
const float initialZoom = 10;

// Camera position
float cameraPos[] = { initialZoom, initialZoom / 2, initialZoom };

// Lighting values with variable positions
float lightPos1[] = { 1, initialZoom, initialZoom, 1 };
float lightPos2[] = { initialZoom, initialZoom, 1, 1 };
float lightAmb[] = { 0.35, 0.35, 0.35, 1 };
float lightDif[] = { 0.35, 0.35, 0.35, 1 };
float lightSpc[] = { 0.35, 0.35, 0.35, 1 };

// Currently selected material
static Material material = TURQUOISE;

// Nudge value in vector by given direction/amount with constraints
void applyChange(float * vec, Direction direction,
	float amount, float min = -FLT_MAX, float max = FLT_MAX)
{
	switch (direction)
	{
	case LEFT:
		if (vec[X] >= min + amount)
			vec[X] -= amount;
		break;
	case RIGHT:
		if (vec[X] <= max - amount)
			vec[X] += amount;
		break;
	case DOWN:
		if (vec[Y] >= min + amount)
			vec[Y] -= amount;
		break;
	case UP:
		if (vec[Y] <= max - amount)
			vec[Y] += amount;
		break;
	case BACK:
		if (vec[Z] >= min + amount)
			vec[Z] -= amount;
		break;
	case FRONT:
		if (vec[Z] <= max - amount)
			vec[Z] += amount;
		break;
	}
}

// Draw planes along 3 axes
void drawAxisBounds()
{
	// y/z plane on boundary of display area
	plainColorMaterial(0.45, 0.5, 0.5);
	glNormal3f(1, 0, 0);
	glBegin(GL_QUADS);
		glVertex3f(0, 0, 0);
		glVertex3f(0, axisLength, 0);
		glVertex3f(0, axisLength, axisLength);
		glVertex3f(0, 0, axisLength);
	glEnd();

	// x/y plane on boundary of display area
	plainColorMaterial(0.5, 0.5, 0.45);
	glNormal3f(0, 0, 1);
	glBegin(GL_QUADS);
		glVertex3f(0, 0, 0);
		glVertex3f(axisLength, 0, 0);
		glVertex3f(axisLength, axisLength, 0);
		glVertex3f(0, axisLength, 0);
	glEnd();

	// x/z plane on boundary of display area
	plainColorMaterial(0.5, 0.45, 0.5);
	glNormal3f(0, 1, 0);
	glBegin(GL_QUADS);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, axisLength);
		glVertex3f(axisLength, 0, axisLength);
		glVertex3f(axisLength, 0, 0);
	glEnd();

	// x axis line separating planes
	plainColorMaterial(0.35, 0, 0);
	glBegin(GL_LINES);
		glVertex3f(0.1, 0.1, 0.1);
		glVertex3f(axisLength, 0.1, 0.1);
	glEnd();

	// y axis line separating planes
	plainColorMaterial(0, 0.35, 0);
	glBegin(GL_LINES);
		glVertex3f(0.1, 0.1, 0.1);
		glVertex3f(0.1, axisLength, 0.1);
	glEnd();

	// z axis line separating planes
	plainColorMaterial(0, 0, 0.35);
	glBegin(GL_LINES);
		glVertex3f(0.1, 0.1, 0.1);
		glVertex3f(0.1, 0.1, axisLength);
	glEnd();
}

// Display function: renders display area
void display()
{
	// Reset display before rendering display area
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// Position camera away from and looking at origin
	gluLookAt(
		cameraPos[X], cameraPos[Y], cameraPos[Z],
		0, 0, 0,
		0, 1, 0
	);

	// Draw planes along 3 axes
	drawAxisBounds();

	// Set light values for first light source
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpc);

	// Set light values for second light source
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDif);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpc);

	// Depict first light source as tiny bright red sphere
	plainColorMaterial(1, 0.75, 0.75);
	glPushMatrix();
		glTranslatef(lightPos1[X], lightPos1[Y], lightPos1[Z]);
		glutSolidSphere(0.1, 100, 100);
	glPopMatrix();

	// Depict second light source as tiny bright blue sphere
	plainColorMaterial(0.75, 0.75, 1);
	glPushMatrix();
		glTranslatef(lightPos2[X], lightPos2[Y], lightPos2[Z]);
		glutSolidSphere(0.1, 100, 100);
	glPopMatrix();

	// Iterate through linear scene graph and draw objects
	for (int i = 0; i < objects.size(); i++)
		objects.at(i).drawObject(selected == i);

	// Flushes buffered commands to display
	glFlush();

	// Redraw display area continuously
	glutPostRedisplay();
}

// Keyboard function: handles standard keyboard controls
void keyboard(unsigned char key, int x, int y)
{
	// Perform action depending on character received
	switch (key)
	{
	// Add objects or choose material/texture
	case '1':
		if (glutGetModifiers() == GLUT_ACTIVE_ALT)
			objects.push_back(Object(material, CUBE));
		else
			material = TURQUOISE;
		break;
	case '2':
		if (glutGetModifiers() == GLUT_ACTIVE_ALT)
			objects.push_back(Object(material, SPHERE));
		else
			material = RED_RUBBER;
		break;
	case '3':
		if (glutGetModifiers() == GLUT_ACTIVE_ALT)
			objects.push_back(Object(material, CONE));
		else
			material = GREEN_PLASTIC;
		break;
	case '4':
		if (glutGetModifiers() == GLUT_ACTIVE_ALT)
			objects.push_back(Object(material, CYLINDER));
		else
			material = PERL;
		break;
	case '5':
		if (glutGetModifiers() == GLUT_ACTIVE_ALT)
			objects.push_back(Object(material, TORUS));
		else
			material = OBSIDIAN;
		break;
	case '6':
		if (glutGetModifiers() == GLUT_ACTIVE_ALT)
			objects.push_back(Object(material, TEAPOT));
		else
			material = TIN;
		break;
	case '7':
		if (glutGetModifiers() == GLUT_ACTIVE_ALT)
			objects.push_back(Object(material, TETRAHEDRON));
		else
			material = POLISHED_BRONZE;
		break;
	case '8':
		if (glutGetModifiers() == GLUT_ACTIVE_ALT)
			objects.push_back(Object(material, OCTAHEDRON));
		else
			material = CARPET;
		break;
	case '9':
		if (glutGetModifiers() == GLUT_ACTIVE_ALT)
			objects.push_back(Object(material, DODECAHEDRON));
		else
			material = MARBLE;
		break;
	case '0':
		if (glutGetModifiers() == GLUT_ACTIVE_ALT)
			objects.push_back(Object(material, ICOSAHEDRON));
		else
			material = ROCK;
		break;

	// Change material of selected object
	case 'm':
	case 'M':
		if (selected != -1)
			objects.at(selected).material = material;
		break;

	// Quit program
	case 'q':
	case 'Q':
	case 27:
		exit(0);
		break;

	// Clear the scene graph of objects
	case 'r':
	case 'R':
		objects.clear();
		selected = -1;
		break;
	}
}

// Special function: handle special keyboard controls
void special(int key, int x, int y)
{
	// Assign direction from arrow key pressed
	switch (key)
	{
	case GLUT_KEY_LEFT:
		direction = LEFT;
		break;
	case GLUT_KEY_RIGHT:
		direction = RIGHT;
		break;
	case GLUT_KEY_DOWN:
		direction = DOWN;
		break;
	case GLUT_KEY_UP:
		direction = UP;
		break;
	case GLUT_KEY_PAGE_DOWN:
		direction = BACK;
		break;
	case GLUT_KEY_PAGE_UP:
		direction = FRONT;
		break;
	default:
		direction = NONE;
		break;
	}

	// Do nothing if not a directional key or no object selected
	if (direction == NONE || selected == -1)
		return;

	// Perform action depending on direction and key modifiers
	if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
		applyChange(objects.at(selected).rotation, direction, 1);
	else if (glutGetModifiers() == GLUT_ACTIVE_ALT)
		applyChange(objects.at(selected).scale,
			direction, 0.5, 0.5, axisLength);
	else if (glutGetModifiers() == GLUT_ACTIVE_SHIFT)
		applyChange(objects.at(selected).position,
			direction, 1, 0, axisLength);
	else if (glutGetModifiers() == (GLUT_ACTIVE_CTRL|GLUT_ACTIVE_SHIFT))
		applyChange(lightPos1, direction, 1, 1, axisLength);
	else if (glutGetModifiers() == (GLUT_ACTIVE_ALT|GLUT_ACTIVE_SHIFT))
		applyChange(lightPos2, direction, 1, 1, axisLength);
	else
		applyChange(cameraPos, direction, 1, 2, axisLength);
}

// Reshape function: adjusts view upon resize of window
void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, axisLength * 2);
	glMatrixMode(GL_MODELVIEW);
}

// Main function: entry point and initialization
int main(int argc, char ** argv)
{
	// Display camera controls to command line
	printf("Direction controls (using keyboard) referenced below:\n"
		"  Left/Right Arrows -> Perform action along x axis (left/right)\n"
		"     Down/Up Arrows -> Perform action along y axis (down/up)\n"
		"       Page Down/Up -> Perform action along z axis (back/front)\n"
		"\nSimple keyboard controls:\n"
		"        Number -> Choose material/texture for new objects\n"
		"  Alt + Number -> New object at world origin\n"
		"        L      -> Load previously saved scene from file\n"
		"        M      -> Apply material/texture to selected object\n"
		"        Q/Esc  -> Quit the program\n"
		"        R      -> Reset scene (delete all objects)\n"
		"        S      -> Save current scene to file\n"
		"\nDirectional keyboard controls:\n"
		"                 Direction -> Move camera\n"
		"         Ctrl  + Direction -> Rotate selected object\n"
		"         Alt   + Direction -> Scale selected object\n"
		"         Shift + Direction -> Translate selected object\n"
		"  Ctrl + Shift + Direction -> Move first light source\n"
		"  Alt  + Shift + Direction -> Move second light source\n"
		"\nMouse controls (anywhere on display window):\n"
		"  Left mouse click  -> Select object under cursor\n"
		"  Right mouse click -> Delete object under cursor\n");

	// GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);

	// Set dimensions and position of window
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(
		(glutGet(GLUT_SCREEN_WIDTH) - 500) / 2,
		(glutGet(GLUT_SCREEN_HEIGHT) - 500) / 2);

	// Create window for displaying modeler
	glutCreateWindow("3GC3 - Assignment 4");

	// Temporary object for testing
	// Object o = Object(material, CUBE);
	// Object o = Object(material, SPHERE);
	// Object o = Object(material, CONE);
	// Object o = Object(material, CYLINDER);
	// Object o = Object(material, TORUS);
	// Object o = Object(material, TEAPOT);
	// Object o = Object(material, TETRAHEDRON);
	// Object o = Object(material, OCTAHEDRON);
	// Object o = Object(material, DODECAHEDRON);
	Object o = Object(material, ICOSAHEDRON);
	o.setPosition(3, 3, 3);
	o.setRotation(30, 30, 30);
	o.setScale(2, 2, 2);
	o.material = TURQUOISE;
	objects.push_back(o);

	// I/O function bindings
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);

	// Enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);

	// Enable backface culling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Initialize texturing
	initTexture();

	// Main program loop
	glutMainLoop();

	// Exit normally
	return 0;
}