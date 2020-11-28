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
	float bounds[8][3];

	// Constructor
	Object(Material m, ObjectType o)
	{
		setPosition(0.5, 0.5, 0.5);
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

		glPushMatrix();
			// Perform transformation before drawing
			glTranslatef(position[X], position[Y], position[Z]);
			glScalef(scale[X], scale[Y], scale[Z]);
			glRotatef(rotation[X], 1, 0, 0);
			glRotatef(rotation[Y], 0, 1, 0);
			glRotatef(rotation[Z], 0, 0, 1);

			// Bind texture before drawing
			glPushMatrix();
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

			// Draw unit-sized object based on type
			switch (objectType)
			{
			case CUBE:
				glutSolidCube(1);
				break;
			case SPHERE:
				glutSolidSphere(0.5, 100, 100);
				break;
			case CONE:
				glRotatef(-90, 1, 0, 0);
				glTranslatef(0, 0, -0.5);
				glutSolidCone(0.5, 1, 100, 100);
				break;
			case CYLINDER:
				glRotatef(-90, 1, 0, 0);
				glTranslatef(0, 0, -0.5);
				glutSolidCylinder(0.5, 1, 100, 100);
				break;
			case TORUS:
				glRotatef(-90, 1, 0, 0);
				glutSolidTorus(0.2, 0.35, 100, 100);
				break;
			case TEAPOT:
				glFrontFace(GL_CW);
				glutSolidTeapot(0.5);
				glFrontFace(GL_CCW);
				break;
			case TETRAHEDRON:
				glRotatef(90, 0, 0, 1);
				glTranslatef(-0.25, -0.125, 0);
				glScalef(0.75, 0.75, 0.75);
				glutSolidTetrahedron();
				break;
			case OCTAHEDRON:
				glScalef(0.6, 0.6, 0.6);
				glutSolidOctahedron();
				break;
			case DODECAHEDRON:
				glScalef(0.4, 0.4, 0.4);
				glutSolidDodecahedron();
				break;
			case ICOSAHEDRON:
				glScalef(0.6, 0.6, 0.6);
				glutSolidIcosahedron();
				break;
			}

			// Unbind texture after drawing
			glBindTexture(GL_TEXTURE_2D, 0);
			glPopMatrix();

			// Draw bounding box if selected
			if (selected)
				drawBounds();
		glPopMatrix();
	}

	// Draw bounding box outline around object
	void drawBounds()
	{
		// Set to outline and disable culling
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);

		// Define green color material for outline
		float green[4] = { 0, 1, 0, 1 };
		float zero[4] = { 0, 0, 0, 0 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, green);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, zero);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0);

		// Define unit square
		float square[8][3] =
		{
			{ -0.5, -0.5, -0.5 },
			{  0.5, -0.5, -0.5 },
			{ -0.5,  0.5, -0.5 },
			{  0.5,  0.5, -0.5 },
			{ -0.5, -0.5,  0.5 },
			{  0.5, -0.5,  0.5 },
			{ -0.5,  0.5,  0.5 },
			{  0.5,  0.5,  0.5 }
		};

		// Bottom two triangles
		glBegin(GL_TRIANGLES);
			glVertex3fv(square[BACK_BOTTOM_LEFT]);
			glVertex3fv(square[FRONT_BOTTOM_LEFT]);
			glVertex3fv(square[FRONT_BOTTOM_RIGHT]);
		glEnd();
		glBegin(GL_TRIANGLES);
			glVertex3fv(square[BACK_BOTTOM_LEFT]);
			glVertex3fv(square[FRONT_BOTTOM_RIGHT]);
			glVertex3fv(square[BACK_BOTTOM_RIGHT]);
		glEnd();

		// Top two triangles
		glBegin(GL_TRIANGLES);
			glVertex3fv(square[BACK_TOP_LEFT]);
			glVertex3fv(square[FRONT_TOP_LEFT]);
			glVertex3fv(square[FRONT_TOP_RIGHT]);
		glEnd();
		glBegin(GL_TRIANGLES);
			glVertex3fv(square[BACK_TOP_LEFT]);
			glVertex3fv(square[FRONT_TOP_RIGHT]);
			glVertex3fv(square[BACK_TOP_RIGHT]);
		glEnd();

		// Back two triangles
		glBegin(GL_TRIANGLES);
			glVertex3fv(square[BACK_BOTTOM_LEFT]);
			glVertex3fv(square[BACK_BOTTOM_RIGHT]);
			glVertex3fv(square[BACK_TOP_RIGHT]);
		glEnd();
		glBegin(GL_TRIANGLES);
			glVertex3fv(square[BACK_BOTTOM_LEFT]);
			glVertex3fv(square[BACK_TOP_RIGHT]);
			glVertex3fv(square[BACK_TOP_LEFT]);
		glEnd();

		// Front two triangles
		glBegin(GL_TRIANGLES);
			glVertex3fv(square[FRONT_BOTTOM_LEFT]);
			glVertex3fv(square[FRONT_BOTTOM_RIGHT]);
			glVertex3fv(square[FRONT_TOP_RIGHT]);
		glEnd();
		glBegin(GL_TRIANGLES);
			glVertex3fv(square[FRONT_BOTTOM_LEFT]);
			glVertex3fv(square[FRONT_TOP_RIGHT]);
			glVertex3fv(square[FRONT_TOP_LEFT]);
		glEnd();

		// Left two triangles
		glBegin(GL_TRIANGLES);
			glVertex3fv(square[BACK_BOTTOM_LEFT]);
			glVertex3fv(square[BACK_TOP_LEFT]);
			glVertex3fv(square[FRONT_TOP_LEFT]);
		glEnd();
		glBegin(GL_TRIANGLES);
			glVertex3fv(square[BACK_BOTTOM_LEFT]);
			glVertex3fv(square[FRONT_TOP_LEFT]);
			glVertex3fv(square[FRONT_BOTTOM_LEFT]);
		glEnd();

		// Right two triangles
		glBegin(GL_TRIANGLES);
			glVertex3fv(square[BACK_BOTTOM_RIGHT]);
			glVertex3fv(square[BACK_TOP_RIGHT]);
			glVertex3fv(square[FRONT_TOP_RIGHT]);
		glEnd();
		glBegin(GL_TRIANGLES);
			glVertex3fv(square[BACK_BOTTOM_RIGHT]);
			glVertex3fv(square[FRONT_TOP_RIGHT]);
			glVertex3fv(square[FRONT_BOTTOM_RIGHT]);
		glEnd();

		// Reset polygon mode and culling
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
	}
};