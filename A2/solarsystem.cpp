// Standard C++ library imports
#include <stdio.h>
#include <stdlib.h>

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

// Global variable for camera position
float camPos[] = {0, 0, 80};

// Draws a sphere (DO NOT CHANGE THIS FUNCTION)
void drawSphere()
{
	glutSolidSphere(1,10,10);
}

// Initialization function
void init(void)
{
	// Initial black background and white color
	glClearColor(0, 0, 0, 0);
	glColor3f(1, 1, 1);

	// Load identity matrix for projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Sets projection to perspective
	gluPerspective(45, 1, 1, 100);
}

// Display function: GLUT display callback function
// clears the screen, sets the camera position,
// draws the groundplane and movable box
void display(void)
{
	// Clear OpenGL color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Load identity matrix for projection
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set camera position and initial color
	gluLookAt(camPos[0], camPos[1], camPos[2], 0, 0, 0, 0,1,0);
	glColor3f(1,1,1);

	// Draw the sun!
	glPushMatrix();
		glColor3f(1,1,0);
		glScalef(2,2,1);
		drawSphere();
	glPopMatrix();

	// Flush out to single buffer
	glFlush();
}

// Main function: program entry point and initialization
int main(int argc, char** argv)
{
	// GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);

	// Set initial window size
	glutInitWindowSize(600, 600);

	// Create window for displaying solar system
	glutCreateWindow("3GC3 - Assignment 2 (Part B)");

	// I/O function bindings
	glutDisplayFunc(display);

	// Call initialization logic
	init();

	// Main program loop
	glutMainLoop();

	// Exit normally
	return(0);
}
