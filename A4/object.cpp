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

// Local library import
#include "material.cpp"

// Enum for supported object types
enum ObjectType
{
	CUBE, SPHERE, CONE, CYLINDER, TORUS, TEAPOT,
	TETRAHEDRON, OCTAHEDRON, DODECAHEDRON, ICOSAHEDRON
};

// Enum for bounding box indices (better readability)
enum BoxCorner
{
	BACK_BOTTOM_LEFT, BACK_BOTTOM_RIGHT, BACK_TOP_LEFT, BACK_TOP_RIGHT,
	FRONT_BOTTOM_LEFT, FRONT_BOTTOM_RIGHT, FRONT_TOP_LEFT, FRONT_TOP_RIGHT
};

// Enum for coordinates (better readability)
enum Coordinate { X, Y, Z, A };

// Struct for objects
struct Object
{
	// Object properties
	float position[3];
	float rotation[3];
	float scale[3];
	Material material;
	ObjectType objectType;

	// Constructor
	Object(Material m, ObjectType o)
	{
		setPosition(1, 1, 1);
		setRotation(0, 0, 0);
		setScale(1, 1, 1);
		material = m;
		objectType = o;
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

	// Bounding sphere radius
	float boundRadius()
	{
		// Set to 3/4 average of scale factor
		return (scale[X] + scale[Y] + scale[Z]) / 4;
	}

	// Draw object based on properties + bounding box if selected
	void drawObject(int selected = 0)
	{
		// Set material values
		switch (material)
		{
		case TURQUOISE:
			setMaterialAmb(0.10, 0.19, 0.17, 0.80);
			setMaterialDif(0.40, 0.74, 0.69, 0.80);
			setMaterialSpc(0.30, 0.31, 0.31, 0.80);
			setMaterialShn(12.8);
			break;
		case RED_RUBBER:
			setMaterialAmb(0.05, 0.00, 0.00, 1.00);
			setMaterialDif(0.50, 0.40, 0.40, 1.00);
			setMaterialSpc(0.70, 0.04, 0.04, 1.00);
			setMaterialShn(10.0);
			break;
		case GREEN_PLASTIC:
			setMaterialAmb(0.02, 0.17, 0.02, 0.55);
			setMaterialDif(0.08, 0.61, 0.08, 0.55);
			setMaterialSpc(0.63, 0.73, 0.63, 0.55);
			setMaterialShn(76.8);
			break;
		case PERL:
			setMaterialAmb(0.25, 0.21, 0.21, 0.92);
			setMaterialDif(1.00, 0.83, 0.83, 0.92);
			setMaterialSpc(0.30, 0.30, 0.30, 0.92);
			setMaterialShn(11.3);
			break;
		case OBSIDIAN:
			setMaterialAmb(0.05, 0.05, 0.07, 0.82);
			setMaterialDif(0.18, 0.17, 0.23, 0.82);
			setMaterialSpc(0.33, 0.33, 0.35, 0.82);
			setMaterialShn(38.4);
			break;
		case TIN:
			setMaterialAmb(0.11, 0.06, 0.11, 1.00);
			setMaterialDif(0.43, 0.47, 0.54, 1.00);
			setMaterialSpc(0.33, 0.33, 0.52, 1.00);
			setMaterialShn(9.85);
			break;
		case POLISHED_BRONZE:
			setMaterialAmb(0.25, 0.15, 0.06, 1.00);
			setMaterialDif(0.40, 0.24, 0.10, 1.00);
			setMaterialSpc(0.77, 0.46, 0.20, 1.00);
			setMaterialShn(76.8);
			break;
		case CARPET:
		case MARBLE:
		case ROCK:
			plainColorMaterial(0.5, 0.5, 0.5);
			break;
		}

		// Push matrix for object transformations
		glPushMatrix();
			// Perform transformation before drawing
			glTranslatef(position[X], position[Y], position[Z]);
			glScalef(scale[X], scale[Y], scale[Z]);
			glRotatef(rotation[X], 1, 0, 0);
			glRotatef(rotation[Y], 0, 1, 0);
			glRotatef(rotation[Z], 0, 0, 1);

			// Bind texture before drawing
			switch (material)
			{
			case CARPET:
				glBindTexture(GL_TEXTURE_2D, textures[0]);
				break;
			case MARBLE:
				glBindTexture(GL_TEXTURE_2D, textures[1]);
				break;
			case ROCK:
				glBindTexture(GL_TEXTURE_2D, textures[2]);
				break;
			default:
				glBindTexture(GL_TEXTURE_2D, 0);
				break;
			}

			// Push matrix for shape-specific transformations
			glPushMatrix();

			// Draw unit-sized object based on type
			switch (objectType)
			{
			case CUBE:
				glutSolidCube(0.9);
				break;
			case SPHERE:
				glutSolidSphere(0.70, 100, 100);
				break;
			case CONE:
				glRotatef(-90, 1, 0, 0);
				glTranslatef(0, 0, -0.25);
				glutSolidCone(0.6, 1, 100, 100);
				break;
			case CYLINDER:
				glRotatef(-90, 1, 0, 0);
				glTranslatef(0, 0, -0.5);
				glutSolidCylinder(0.5, 1, 100, 100);
				break;
			case TORUS:
				glRotatef(-90, 1, 0, 0);
				glutSolidTorus(0.25, 0.45, 100, 100);
				break;
			case TEAPOT:
				glFrontFace(GL_CW);
				glutSolidTeapot(0.5);
				glFrontFace(GL_CCW);
				break;
			case TETRAHEDRON:
				glRotatef(90, 0, 0, 1);
				glTranslatef(0, 0, 0);
				glScalef(0.75, 0.75, 0.75);
				glutSolidTetrahedron();
				break;
			case OCTAHEDRON:
				glScalef(0.75, 0.75, 0.75);
				glutSolidOctahedron();
				break;
			case DODECAHEDRON:
				glScalef(0.4, 0.4, 0.4);
				glutSolidDodecahedron();
				break;
			case ICOSAHEDRON:
				glScalef(0.75, 0.75, 0.75);
				glutSolidIcosahedron();
				break;
			}

			// Pop matrix for shape-specific transformations
			glPopMatrix();

			// Unbind texture after drawing
			glBindTexture(GL_TEXTURE_2D, 0);

		// Pop matrix for object transformations
		glPopMatrix();

		// No bounding sphere if not selected
		if (!selected)
			return;

		// Draw bounding sphere (upright around object)
		glPushMatrix();
			float radius = boundRadius();
			plainColorMaterial(0, 1, 0);
			glTranslatef(position[X], position[Y], position[Z]);
			glScalef(radius, radius, radius);
			glRotatef(90, 1, 0, 0);
			glutWireSphere(1, 16, 8);
		glPopMatrix();
	}
};