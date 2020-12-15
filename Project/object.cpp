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

// Local imports
#include "material.cpp"
#include "object.h"
#include "objLoader.h"

// Loading object files
objLoader *rock = new objLoader((char*)"Objects/rock.obj");
objLoader *bomb = new objLoader((char*)"Objects/bomb.obj");
objLoader *coin = new objLoader((char*)"Objects/coin.obj");

// Generic object constructor (set coordinates)
Object::Object(float x, float y, float z)
{
	this->positionX = x;
	this->positionY = y;
	this->positionZ = z;
	this->collided = 0;
	this->rotation = 0;
	this->intensity = 1;
	for (int i = 0; i < NUM_PARTICLES; i++)
		this->particles[i] = Particle();
}

// Generic object renderer
void Object::render(float r, float g, float b, float s)
{
	// Position object
	glPushMatrix();
	glColor3f(r * this->intensity,
		g * this->intensity, b * this->intensity);
	glTranslatef(this->positionX, this->positionY, this->positionZ);
	glRotatef(this->rotation, 0, 1, 0);

	// Scale and draw object unless collided
	if (!this->collided)
	{
		if (s == 1)
		{
			glScalef(0.3, 0.3, 0.3);
			rock->drawObj();
		}
		else if (s == 2)
		{
			glScalef(0.08, 0.08, 0.08);
			bomb->drawObj();
		}
		else
		{
			glScalef(0.2, 0.2, 0.2);
			coin->drawObj();
		}
	}
	glPopMatrix();

	// Render particles during collision
	float c[3] = { r, g, b };
	if (this->collided)
	{
		for (int i = 0; i < NUM_PARTICLES; i++)
		{
			if (!particles[i].exists)
				particles[i].position(this->positionX,
					this->positionY, this->positionZ);
			this->particles[i].render(c);
		}
	}
}

// Rocket constructor (parent constructor, rocket type identifier)
Rocket::Rocket(float x, float y, float z) : Object(x, y, z)
{
	this->type = ROCKET;
}

// Random number generator function used for fire
float Rocket::random(int min, int max)
{
nextRand:
	int i = (rand() % 100) / 10;
	if(max < 7)
	{
		if(i == 8 || i == 7)
			i = -1;
		else if(i == 9)
			i = -2;
	}

	if(i >= min && i <= max)
		return i;
	else
		goto nextRand;
}

// Creates fire illusion for rocket
void Rocket::fireFunc()
{
	glPushMatrix();
		for (int i = 0; i < 180; i++)
		{
			glPushMatrix();
				glTranslated(
					this->positionX + Rocket::random(-5, 5) - 1.3,
					this->positionY - (19.5 + Rocket::random(0, 30)),
					this->positionZ + Rocket::random(0, 5) + 8);
				glTranslatef(0, 0, -75);
				if(i % 3 == 0)
				{
					glColor3fv(fireColor1);
					glutWireTetrahedron();
				}
				else if(i % 2 == 0)
				{
					glColor3fv(fireColor2);
					glutWireSphere(0.6, 30, 30);
				}
				else
				{
					glColor3fv(fireColor3);
					glutSolidOctahedron();
				}
			glPopMatrix();
		}
		glPopMatrix();
}

// Separate logic for drawing rocket
void Rocket::drawRocket()
{
	// Make rocket appear tilted upwards
	glTranslatef(-3.5, 0, -60);
	glRotatef(-70, 1, 0, 0);
	glTranslatef(-3.5, 0, 60);

	// Initial colour
	glColor3fv(offwhite);

	// GLUquadric initializations
	GLUquadricObj *rocketobj = gluNewQuadric();
	gluQuadricDrawStyle(rocketobj, GLU_FILL);
	gluQuadricNormals(rocketobj, GLU_SMOOTH);
	gluQuadricOrientation(rocketobj, GLU_OUTSIDE);
	gluQuadricTexture(rocketobj, GLU_TRUE);

	// Front peak of rocket
	glPushMatrix ();
		glTranslatef(7, 13, -71);
		glutSolidSphere(2, 40, 40);
	glPopMatrix();

	// Front cylinder of rocket
	glPushMatrix ();
		glTranslatef(7,7, -71);
		glRotatef(-90, 1, 0, 0);
		gluCylinder(rocketobj, 4, 2, 6.5, 40, 40);
	glPopMatrix();

	// Body of rocket
	glPushMatrix();
		glTranslatef(7, -12, -71);
		glRotatef(-90, 1, 0, 0);
		gluCylinder(rocketobj, 4, 4, 19, 40, 40);
	glPopMatrix();

	// Three windows
	glColor3fv(darkgrey);
	// { TranslateX, TranslateY, TranslateZ, RotateY }
	float Windows[3][4] =
		{ {7, 9, 10.9, 0}, {5.5, 8.4, 10.7, 1}, {8.5, 8.4, 10.7, -1} };
	for(int i = 0; i < 3; i++) // Middle, left then right window
	{
		glPushMatrix ();
			glTranslatef(Windows[i][0], Windows[i][1], Windows[i][2]-80);
			glRotatef(90, 1, Windows[i][3], 0);
			gluCylinder(rocketobj, 1, 0.5, 1, 40, 40);
		glPopMatrix();
	}

	// Creates fins of rocket
	glBindTexture(GL_TEXTURE_2D, 0);
	float Fins[3][4] =
		{ {-6, 10, 1, 0.5}, {-11, 5, 1.9, 1}, {-14, 3, 2.1, 1.9} };
	glColor3fv(nearblack);
	for(int i = 0; i < 3; i++) // Top, middle then bottom fin
	{
		glPushMatrix ();
			glTranslatef(7, Fins[i][0], -72);
			glScalef(3.5, Fins[i][1], 0);
			glRotated(-90, 1, 0, 0);
			gluCylinder(rocketobj, Fins[i][2], Fins[i][3], 1, 40, 40);
		glPopMatrix();
	}
}

// Rocket renderer (separate rendering logic)
void Rocket::render()
{
	glPushMatrix();
		glTranslatef(this->positionX, this->positionY, this->positionZ);
		if (!this->collided)
			drawRocket();
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
	Object::render(1, 1, 0, 3);
}

// Rock constructor (parent constructor, rock type identifier)
Obstacle::Obstacle(float x, float y, float z) : Object(x, y, z)
{
	this->type = OBSTACLE;
}

// Rock renderer (parent renderer with parameters)
void Obstacle::render()
{
	Object::render(0.5, 0.5, 0.5, 1);
}

// Bomb constructor (parent constructor, bomb type identifier)
Bomb::Bomb(float x, float y, float z) : Object(x, y, z)
{
	this->type = BOMB;
}

// Bomb renderer (parent renderer with parameters)
void Bomb::render()
{
	Object::render(0.3, 0, 0, 2);
}
