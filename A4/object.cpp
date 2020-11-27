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
		this->setPosition(0.5, 0.5, 0.5);
		this->setRotation(0, 0, 0);
		this->setScale(1, 1, 1);
		this->setMaterialAmb(0.25, 0.25, 0.25, 1);
		this->setMaterialDif(0.25, 0.25, 0.25, 1);
		this->setMaterialSpc(0.25, 0.25, 0.25, 1);
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

	// Draw object based on properties + bounding box if selected
	void drawObject(int selected = 0)
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
				glutSolidSphere(0.5, 100, 100);
				break;
			case CONE:
				glPushMatrix();
					glRotatef(-90, 1, 0, 0);
					glTranslatef(0, 0, -0.5);
					glutSolidCone(0.5, 1, 100, 100);
				glPopMatrix();
				break;
			case CYLINDER:
				glPushMatrix();
					glRotatef(-90, 1, 0, 0);
					glTranslatef(0, 0, -0.5);
					glutSolidCylinder(0.5, 1, 100, 100);
				glPopMatrix();
				break;
			case TORUS:
				glPushMatrix();
					glRotatef(-90, 1, 0, 0);
					glutSolidTorus(0.2, 0.35, 100, 100);
				glPopMatrix();
				break;
			case TEAPOT:
				glFrontFace(GL_CW);
				glutSolidTeapot(0.5);
				glFrontFace(GL_CCW);
				break;
			case TETRAHEDRON:
				glPushMatrix();
					glRotatef(90, 0, 0, 1);
					glTranslatef(-0.25, -0.125, 0);
					glScalef(0.75, 0.75, 0.75);
					glutSolidTetrahedron();
				glPopMatrix();
				break;
			case OCTAHEDRON:
				glPushMatrix();
					glScalef(0.6, 0.6, 0.6);
					glutSolidOctahedron();
				glPopMatrix();
				break;
			case DODECAHEDRON:
				glPushMatrix();
					glScalef(0.4, 0.4, 0.4);
					glutSolidDodecahedron();
				glPopMatrix();
				break;
			case ICOSAHEDRON:
				glPushMatrix();
					glScalef(0.6, 0.6, 0.6);
					glutSolidIcosahedron();
				glPopMatrix();
				break;
			}
			if (selected)
				this->drawBounds();
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