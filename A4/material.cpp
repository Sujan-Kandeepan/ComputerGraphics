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
	OBSIDIAN, TIN, POLISHED_BRONZE,
	CARPET, MARBLE, ROCK
};

// Global texture variables
static GLuint textures[3];
static GLubyte *carpet, *marble, *rock;
static int width, height, max;

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

// PPM loader taken from texture.cpp in lecture code (printf removed)
static GLubyte *LoadPPM(char *file, int *width, int *height, int *max)
{
    GLubyte *img;
    FILE *fd;
    int n, m;
    int k, nm;
    char c;
    int i;
    char b[100];
    float s;
    int red, green, blue;

    fd = fopen(file, "r");
    fscanf(fd, "%[^\n] ", b);
    if (b[0] != 'P' || b[1] != '3')
    {
        exit(0);
    }
    fscanf(fd, "%c", &c);
    while (c == '#')
    {
        fscanf(fd, "%[^\n] ", b);
        printf("%s\n", b);
        fscanf(fd, "%c", &c);
    }
    ungetc(c, fd);
    fscanf(fd, "%d %d %d", &n, &m, &k);

    nm = n * m;

    img = (GLubyte *)(malloc(3 * sizeof(GLuint) * nm));

    s = 255.0 / k;

    for (i = 0; i < nm; i++)
    {
        fscanf(fd, "%d %d %d", &red, &green, &blue);
        img[3 * nm - 3 * i - 3] = red * s;
        img[3 * nm - 3 * i - 2] = green * s;
        img[3 * nm - 3 * i - 1] = blue * s;
    }

    *width = n;
    *height = m;
    *max = k;

    return img;
}

// Initialization logic for textures
static void initTexture()
{
	// Enable texturing
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glGenTextures(3, textures);

	// Set up first texture for carpet
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	carpet = LoadPPM((char*)"carpet.ppm", &width, &height, &max);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
		0, GL_RGB, GL_UNSIGNED_BYTE, carpet);

	// Set up second texture for marble
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	marble = LoadPPM((char*)"marble.ppm", &width, &height, &max);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
		0, GL_RGB, GL_UNSIGNED_BYTE, marble);

	// Set up third texture for rock
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	rock = LoadPPM((char*)"rock.ppm", &width, &height, &max);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
		0, GL_RGB, GL_UNSIGNED_BYTE, rock);

	// Remove texture binding for now
	glBindTexture(GL_TEXTURE_2D, 0);
}

#endif