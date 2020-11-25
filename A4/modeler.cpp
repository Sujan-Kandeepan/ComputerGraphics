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

// Enum for supported object types
enum ObjectType
{
	CUBE, SPHERE, CONE, CYLINDER, TORUS, TEAPOT,
	TETRAHEDRON, OCTAHEDRON, DODECAHEDRON, ICOSAHEDRON
};

// Enum for bounding box indices (better readability)
enum BoxCorners
{
	BACK_BOTTOM_LEFT, BACK_BOTTOM_RIGHT, BACK_TOP_LEFT, BACK_TOP_RIGHT,
	FRONT_BOTTOM_LEFT, FRONT_BOTTOM_RIGHT, FRONT_TOP_LEFT, FRONT_TOP_RIGHT
};

// Enum for coordinates (better readability)
enum Coordinates { X, Y, Z, A };

// Struct for objects
struct Object
{
	// Object properties
	float position[3];
	float rotation[4];
	float scale[3];
	float materialAmb[4];
	float materialDif[4];
	float materialSpc[4];
	float materialShiny;
	ObjectType objectType;
	float bounds[8][3];

	// Constructor
	Object(){}

	// Set position (x, y, z) values
	void setPosition(float x, float y, float z)
	{
		position[X] = x;
		position[Y] = y;
		position[Z] = z;
	}

	// Set rotation (x, y, z) and angle values
	void setRotation(float x, float y, float z, float a)
	{
		rotation[X] = x;
		rotation[Y] = y;
		rotation[Z] = z;
		rotation[A] = a;
	}

	// Set scale (x, y, z) values
	void setScale(float x, float y, float z)
	{
		scale[X] = x;
		scale[Y] = y;
		scale[Z] = z;
	}

	// Set material ambience (x, y, z, a) values
	void setMaterialAmb(float x, float y, float z, float a)
	{
		materialAmb[X] = x;
		materialAmb[Y] = y;
		materialAmb[Z] = z;
		materialAmb[A] = a;
	}

	// Set material diffuse (x, y, z, a) values
	void setMaterialDif(float x, float y, float z, float a)
	{
		materialDif[X] = x;
		materialDif[Y] = y;
		materialDif[Z] = z;
		materialDif[A] = a;
	}

	// Set material specular (x, y, z, a) values
	void setMaterialSpc(float x, float y, float z, float a)
	{
		materialSpc[X] = x;
		materialSpc[Y] = y;
		materialSpc[Z] = z;
		materialSpc[A] = a;
	}

	// Calculate bounding box coordinates
	void calculateBounds()
	{
		// TODO: implement bounding box calculation
	}

	// Draw object of correct type with transformations/material
	void drawObject()
	{
		// TODO: implement object rendering
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialAmb);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDif);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpc);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialShiny);
		glPushMatrix();
			glScalef(scale[X], scale[Y], scale[Z]);
			glRotatef(rotation[A], rotation[X], rotation[Y], rotation[Z]);
			glTranslatef(position[X], position[Y], position[Z]);
			switch (objectType)
			{
			case CUBE:
				glutSolidCube(1);
				break;
			case SPHERE:
				glutSolidSphere(1, 10, 10);
				break;
			case CONE:
				glutSolidCone(1, 1, 10, 10);
				break;
			case CYLINDER:
				glutSolidCone(1, 1, 10, 10); // ???
				break;
			case TORUS:
				glutSolidTorus(0.5, 1, 10, 10);
				break;
			case TEAPOT:
				glutSolidTeapot(1);
				break;
			case TETRAHEDRON:
				glutSolidTetrahedron();
				break;
			case OCTAHEDRON:
				glutSolidOctahedron();
				break;
			case DODECAHEDRON:
				glutSolidDodecahedron();
				break;
			case ICOSAHEDRON:
				glutSolidIcosahedron();
				break;
			}
		glPopMatrix();
	}

	// Draw bounding box outline around object
	void drawBounds()
	{
		// TODO: implement bounding box drawing
	}
};

std::vector<Object> objects;

// Display function: renders display area
void display()
{
	// Reset display before rendering display area
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// Position camera away from and looking at origin
	gluLookAt(
		10, 10, 10,
		0, 0, 0,
		0, 1, 0
	);

	// Temporary lighting setup
	float pos[4] = {10, 10, 10, 1};
	float amb[4] = {1, 1, 1, 1};
	float dif[4] = {0, 0, 0, 1};
	float spc[4] = {0, 0, 0, 1};
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spc);

	// Temporary object for testing
	Object o = Object();
	o.setPosition(0, 0, 0);
	o.setRotation(0, 0, 0, 0);
	o.setScale(1, 1, 1);
	o.setMaterialAmb(0.5, 0.5, 0.5, 1);
	o.setMaterialDif(0, 0, 0, 1);
	o.setMaterialSpc(0, 0, 0, 1);
	o.materialShiny = 1;
	o.objectType = ICOSAHEDRON;
	objects.push_back(o);

	// Iterate through linear scene graph and draw objects
	for (int i = 0; i < objects.size(); i++)
	{
		objects.at(i).drawObject();
	}

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
		// Quit program
		case 'q':
		case 'Q':
		case 27:
			exit(0);
			break;
	}
}

// Reshape function: adjusts view upon resize of window
void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 25);
	glMatrixMode(GL_MODELVIEW);
}

// Main function: entry point and initialization
int main(int argc, char ** argv)
{
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

	// I/O function bindings
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);

	// Enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Enable backface culling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Main program loop
	glutMainLoop();

	// Exit normally
	return 0;
}