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

#ifndef MATERIAL
#define MATERIAL

// Enum for material names
enum Material
{
	TURQUOISE, RED_RUBBER, GREEN_PLASTIC, PERL,
	OBSIDIAN, TIN, POLISHED_BRONZE
};

// Set material ambience (x, y, z, a) values
static void setMaterialAmb(float x, float y, float z, float a)
{
	float amb[4] = { x, y, z, a };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
}

// Set material diffuse (x, y, z, a) values
static void setMaterialDif(float x, float y, float z, float a)
{
	float dif[4] = { x, y, z, a };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
}

// Set material specular (x, y, z, a) values
static void setMaterialSpc(float x, float y, float z, float a)
{
	float spc[4] = { x, y, z, a };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spc);
}

// Set material shine value
static void setMaterialShn(float s)
{
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, s);
}

// Set temporary plain color for objects to be drawn
static void plainColorMaterial(float r, float g, float b)
{
	setMaterialAmb(r, g, b, 1);
	setMaterialDif(r, g, b, 1);
	setMaterialSpc(r, g, b, 1);
	setMaterialShn(1);
}

#endif