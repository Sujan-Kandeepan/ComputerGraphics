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

#ifndef __MATERIAL__
#define __MATERIAL__

using namespace std;

// Global textures
static GLuint textures[3];
static GLubyte *coinT, *rockT, *rocketT;
static int width0, height0, max0;

// Color constants for tunnel display, rocket and text
static const float tunnelLine[] = { 0.15, 0.30, 0.45 };
static const float fireColor1[] = { 1.00, 0.40, 0.00 };
static const float fireColor2[] = { 1.00, 0.60, 0.00 };
static const float fireColor3[] = { 0.90, 0.90, 0.20 };
static const float   offwhite[] = { 0.90, 0.90, 0.90 };
static const float       grey[] = { 0.50, 0.50, 0.50 };
static const float   darkgrey[] = { 0.25, 0.25, 0.25 };
static const float  nearblack[] = { 0.10, 0.10, 0.10 };
static const float        red[] = { 1.00, 0.00, 0.00 };
static const float   lightred[] = { 1.00, 0.50, 0.50 };
static const float     orange[] = { 1.00, 0.75, 0.25 };
static const float      green[] = { 0.00, 1.00, 0.00 };
static const float lightgreen[] = { 0.50, 1.00, 0.50 };
static const float  lightblue[] = { 0.50, 1.00, 1.00 };

// Material structure
struct Material
{
	float ambient[4];
	float diffuse[4];
	float specular[4];
	GLfloat shine;
};

// Tunnel material
static Material tunnel =
{
	{ 0.10, 0.15, 0.20, 1.00 },
	{ 0.25, 0.50, 0.75, 1.00 },
	{ 0.50, 0.75, 1.00, 1.00 },
	100.0
};

// Coin material
static Material gold =
{
	{ 0.25, 0.20, 0.08, 1.00 },
	{ 0.75, 0.61, 0.23, 1.00 },
	{ 0.63, 0.56, 0.37, 1.00 },
	51.2
};

// Rock material
static Material chrome =
{
	{ 0.25, 0.25, 0.25, 1.00 },
	{ 0.40, 0.40, 0.40, 1.00 },
	{ 0.77, 0.77, 0.77, 1.00 },
	76.8
};

// Bomb material
static Material redMatte =
{
	{ 0.00, 0.00, 0.00, 1.00 },
	{ 1.00, 0.00, 0.00, 1.00 },
	{ 0.50, 0.50, 0.50, 1.00 },
	10.0
};

// Set and apply material
static void setMaterial(Material m)
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m.ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m.diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m.specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m.shine);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

// Read PPM files (Week 9 lecture)
static GLubyte* LoadPPM(char* file, int* width, int* height, int* max)
{
	GLubyte* img;
	FILE *fd;
	int n, m;
	int k, nm;
	char c;
	int i;
	char b[100];
	float s;
	int red, green, blue;

	// First open file and check if it's an ASCII PPM
	// (indicated by P3 at the start)
	fd = fopen(file, "r");
	fscanf(fd, "%[^\n] ",b);
	if (b[0] != 'P' || b[1] != '3')
	{
		printf("%s is not a PPM file!\n", file);
		exit(0);
	}
	fscanf(fd, "%c", &c);

	// Next, skip past the comments
	while (c == '#')
	{
		fscanf(fd, "%[^\n] ", b);
		fscanf(fd, "%c", &c);
	}
	ungetc(c, fd);

	// Now get the dimensions and max color value from the image
	fscanf(fd, "%d %d %d", &n, &m, &k);

	// Calculate number of pixels and allocate storage for this
	nm = n * m;
	img = (GLubyte*)malloc(3 * sizeof(GLuint) * nm);
	s = 255.0 / k;

	// For every pixel, grab the read green and blue values,
	// storing them in the image data array
	for (i = 0; i < nm; i++)
	{
		fscanf(fd, "%d %d %d", &red, &green, &blue);
		img[3 * nm - 3 * i - 3] = red * s;
		img[3 * nm - 3 * i - 2] = green * s;
		img[3 * nm - 3 * i - 1] = blue * s;
	}

	// Set the return parameters and return the image array
	*width = n;
	*height = m;
	*max = k;

	return img;
}

// Load textures
static void initTextures()
{
	// Enable textures and auto texture coordinates
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glGenTextures(3, textures);

	// Texture binding and options for coins
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	coinT = LoadPPM((char*)"Textures/coin.ppm",
		&width0, &height0, &max0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width0, height0,
		0, GL_RGB, GL_UNSIGNED_BYTE, coinT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Texture binding and options for rocks
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	rockT = LoadPPM((char*)"Textures/rock.ppm",
		&width0, &height0, &max0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width0, height0,
		0, GL_RGB, GL_UNSIGNED_BYTE, rockT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Texture binding and options for rocket
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	rocketT = LoadPPM((char*)"Textures/rocketT.ppm",
		&width0, &height0, &max0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width0, height0,
		0, GL_RGB, GL_UNSIGNED_BYTE, rocketT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

#endif
