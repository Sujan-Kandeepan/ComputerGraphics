// Standard C++ library imports
#include <stdio.h>

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

// Import header file
#include "object.h"

// Generic object constructor (set coordinates)
Object::Object(float x, float y, float z)
{
	this->positionX = x;
	this->positionY = y;
	this->positionZ = z;
}

// Generic object renderer (obj loader logic)
void Object::render(float r, float g, float b, float s)
{
	// TODO: implement render method
	glPushMatrix();
	glColor3f(r, g, b);
	glTranslatef(this->positionX, this->positionY, this->positionZ);
	glutWireSphere(s, 10, 10);
	glPopMatrix();
}

// Rocket constructor (parent constructor, rocket type identifier)
Rocket::Rocket(float x, float y, float z) : Object(x, y, z)
{
	this->type = ROCKET;
}

// Rocket renderer (parent renderer with parameters)
void Rocket::render()
{
	// Basic for now, to be replaced by obj loader
	glPushMatrix();
		// Considering making rocket half-transparent when in middle
		glColor3f(1, 1, 1);
		glTranslatef(this->positionX, this->positionY, this->positionZ);
		glRotatef(90, 0, 1, 0);
		glutWireTeapot(2);
	glPopMatrix();
}

// Coin constructor (parent constructor, coin type identifier)
Coin::Coin(float x, float y, float z) : Object(x, y, z)
{
	this->type = COIN;
}

// Coin renderer (parent renderer with parameters)
void Coin::render()
{
	// Basic for now, to be replaced by obj loader
	Object::render(1, 1, 0, 0.5);
}

// Obstacle constructor (parent constructor, rocket type identifier)
Obstacle::Obstacle(float x, float y, float z) : Object(x, y, z)
{
	this->type = OBSTACLE;
}

// Obstacle renderer (parent renderer with parameters)
void Obstacle::render()
{
	// Basic for now, to be replaced by obj loader
	Object::render(0.5, 0.5, 1, 0.75);
}

// Bomb constructor (parent constructor, rocket type identifier)
Bomb::Bomb(float x, float y, float z) : Object(x, y, z)
{
	this->type = BOMB;
}

// Bomb renderer (parent renderer with parameters)
void Bomb::render()
{
	// Basic for now, to be replaced by obj loader
	Object::render(0.5, 0, 0, 0.25);
}