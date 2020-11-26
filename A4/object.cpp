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
	float rotation[3];
	float scale[3];
	float materialAmb[4];
	float materialDif[4];
	float materialSpc[4];
	float materialShiny;
	ObjectType objectType;
	float bounds[8][3];

	// Constructor
	Object(ObjectType o)
	{
		this->setPosition(1, 1, 1);
		this->setRotation(0, 0, 0);
		this->setScale(1, 1, 1);
		this->setMaterialAmb(0.25, 0.25, 0.25, 1);
		this->setMaterialDif(0, 0, 0, 1);
		this->setMaterialSpc(0, 0, 0, 1);
		this->materialShiny = 1;
		this->objectType = o;
	}

	// Set position (x, y, z) values
	void setPosition(float x, float y, float z)
	{
		position[X] = x;
		position[Y] = y;
		position[Z] = z;
	}

	// Set rotation values around x/y/z axes
	void setRotation(float x, float y, float z)
	{
		rotation[X] = x;
		rotation[Y] = y;
		rotation[Z] = z;
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
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialAmb);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDif);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpc);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialShiny);
		glPushMatrix();
			glTranslatef(position[X], position[Y], position[Z]);
			glScalef(scale[X], scale[Y], scale[Z]);
			glRotatef(rotation[X], 1, 0, 0);
			glRotatef(rotation[Y], 0, 1, 0);
			glRotatef(rotation[Z], 0, 0, 1);
			switch (objectType)
			{
			case CUBE:
				glutSolidCube(1);
				break;
			case SPHERE:
				glutSolidSphere(1, 100, 100);
				break;
			case CONE:
				glutSolidCone(1, 1, 100, 100);
				break;
			case CYLINDER:
				glutSolidCylinder(1, 1, 100, 100);
				break;
			case TORUS:
				glutSolidTorus(0.5, 1, 100, 100);
				break;
			case TEAPOT:
				glFrontFace(GL_CW);
				glutSolidTeapot(1);
				glFrontFace(GL_CCW);
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