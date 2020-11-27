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

// Object type imports
#include "object.cpp"

// Linear scene graph of objects
std::vector<Object> objects;

// Index of selected object
int selected = -1;

// Constant values for display
const float axisLength = 100;
const float initialZoom = 10;

// Rotation for visualization (temporary)
float rotation = 0;
void rotate(int val)
{
	rotation++;
	glutTimerFunc(1000 / 60, rotate, 0);
}

// Set temporary plain color for objects to be drawn
void plainColorMaterial(float r, float g, float b)
{
	float colorAmb[4] = {r, g, b, 1};
	float colorDif[4] = {r, g, b, 1};
	float colorSpc[4] = {r, g, b, 1};
	float colorShiny = 1;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, colorAmb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, colorDif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, colorSpc);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, colorShiny);
}

// Display function: renders display area
void display()
{
	// Reset display before rendering display area
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// Position camera away from and looking at origin
	gluLookAt(
		initialZoom, initialZoom / 2, initialZoom,
		0, 0, 0,
		0, 1, 0
	);

	// Temporary lighting setup
	float pos[4] = {axisLength, axisLength, axisLength, 1};
	float amb[4] = {0.5, 0.5, 0.5, 1};
	float dif[4] = {0.5, 0.5, 0.5, 1};
	float spc[4] = {0.5, 0.5, 0.5, 1};
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spc);

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

	// Iterate through linear scene graph and draw objects
	if (!objects.empty())
		objects.at(0).setRotation(30, 30 + rotation, 30);
	for (int i = 0; i < objects.size(); i++)
		objects.at(i).drawObject(1);

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
	// Add objects or choose material
	case '1':
		if (glutGetModifiers() & GLUT_ACTIVE_ALT)
			printf("Choose first material\n");
		else
			objects.push_back(Object(CUBE));
		break;
	case '2':
		if (glutGetModifiers() & GLUT_ACTIVE_ALT)
			printf("Choose second material\n");
		else
			objects.push_back(Object(SPHERE));
		break;
	case '3':
		if (glutGetModifiers() & GLUT_ACTIVE_ALT)
			printf("Choose third material\n");
		else
			objects.push_back(Object(CONE));
		break;
	case '4':
		if (glutGetModifiers() & GLUT_ACTIVE_ALT)
			printf("Choose fourth material\n");
		else
			objects.push_back(Object(CYLINDER));
		break;
	case '5':
		if (glutGetModifiers() & GLUT_ACTIVE_ALT)
			printf("Choose fifth material\n");
		else
			objects.push_back(Object(TORUS));
		break;
	case '6':
		if (glutGetModifiers() & GLUT_ACTIVE_ALT)
			printf("Choose first texture\n");
		else
			objects.push_back(Object(TEAPOT));
		break;
	case '7':
		if (glutGetModifiers() & GLUT_ACTIVE_ALT)
			printf("Choose second texture\n");
		else
			objects.push_back(Object(TETRAHEDRON));
		break;
	case '8':
		if (glutGetModifiers() & GLUT_ACTIVE_ALT)
			printf("Choose third texture\n");
		else
			objects.push_back(Object(OCTAHEDRON));
		break;
	case '9':
		if (!(glutGetModifiers() & GLUT_ACTIVE_ALT))
			objects.push_back(Object(DODECAHEDRON));
		break;
	case '0':
		if (!(glutGetModifiers() & GLUT_ACTIVE_ALT))
			objects.push_back(Object(ICOSAHEDRON));
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
		break;
	}
}

// Special function: handle special keyboard controls
void special(int key, int x, int y)
{
	enum Direction { NONE, LEFT, RIGHT, DOWN, UP, BACK, FRONT };
	Direction direction = NONE;

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
	}

	if (direction == NONE)
		return;

	if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
	{
		printf("Rotate selected object\n");
	}
	else if (glutGetModifiers() == GLUT_ACTIVE_ALT)
	{
		printf("Scale selected object\n");
	}
	else if (glutGetModifiers() == GLUT_ACTIVE_SHIFT)
	{
		printf("Translate selected object\n");
	}
	else if (glutGetModifiers() == (GLUT_ACTIVE_CTRL|GLUT_ACTIVE_SHIFT))
	{
		printf("Move first light source\n");
	}
	else if (glutGetModifiers() == (GLUT_ACTIVE_ALT|GLUT_ACTIVE_SHIFT))
	{
		printf("Move second light source\n");
	}
	else
	{
		printf("Move camera\n");
	}
}

// Reshape function: adjusts view upon resize of window
void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, initialZoom * 2);
	glMatrixMode(GL_MODELVIEW);
}

// Main function: entry point and initialization
int main(int argc, char ** argv)
{
	// Display camera controls to command line
	printf("Direction controls (using keyboard) referenced below:\n"
		"Left/Right Arrows -> Perform action along x axis (left/right)\n"
		"Down/Up Arrows    -> Perform action along y axis (down/up)\n"
		"Page Down/Up      -> Perform action along z axis (back/front)\n"
		"\nSimple keyboard controls:\n"
		"Number (1-8 only) -> Choose material/texture for new objects\n"
		"Alt + Number      -> New object at world origin\n"
		"L                 -> Load previously saved scene from file\n"
		"M                 -> Apply chosen material to selected object\n"
		"Q/Esc             -> Quit the program\n"
		"R                 -> Reset scene (delete all objects)\n"
		"S                 -> Save current scene to file\n"
		"\nDirectional keyboard controls:\n"
		"               Direction -> Move camera\n"
		"        Ctrl + Direction -> Rotate selected object\n"
		"         Alt + Direction -> Scale selected object\n"
		"       Shift + Direction -> Translate selected object\n"
		"Ctrl + Shift + Direction -> Move first light source\n"
		" Alt + Shift + Direction -> Move second light source\n"
		"\nMouse controls (anywhere on display window):\n"
		"Left mouse click  -> Select object under cursor\n"
		"Right mouse click -> Delete object under cursor\n");

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
	Object o = Object(ICOSAHEDRON);
	o.setPosition(3, 3, 3);
	o.setRotation(30, 30, 30);
	o.setScale(2, 2, 2);
	o.setMaterialAmb(0.3, 0.2, 0.1, 1);
	o.setMaterialDif(0.8, 0.7, 0.5, 1);
	o.setMaterialSpc(0.2, 0.2, 0.1, 1);
	objects.push_back(o);

	// I/O function bindings
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);

	// Enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);

	// Enable backface culling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Main program loop
	glutTimerFunc(1000 / 60, rotate, 0);
	glutMainLoop();

	// Exit normally
	return 0;
}