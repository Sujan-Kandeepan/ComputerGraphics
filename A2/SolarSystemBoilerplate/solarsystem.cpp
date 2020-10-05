#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

//Globals
float camPos[] = {0, 0, 80};	//where the camera is

/* Draws a sphere
 Do not change this function */
void drawSphere(){
    glutSolidSphere(1,10,10);
}

void init(void)
{
	glClearColor(0, 0, 0, 0);
	glColor3f(1, 1, 1);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    /* Sets projection to perspective */
	gluPerspective(45, 1, 1, 100);
}

/* display function - GLUT display callback function
 *		clears the screen, sets the camera position, draws the ground plane and movable box
 */
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(camPos[0], camPos[1], camPos[2], 0, 0, 0, 0,1,0);
	glColor3f(1,1,1);

	//draw the sun!
    glPushMatrix();
        glColor3f(1,1,0);
        glScalef(2,2,1);
        drawSphere();
    glPopMatrix();
    
	//flush out to single buffer
	glFlush();
}

/* main function - program entry point */
int main(int argc, char** argv)
{
	glutInit(&argc, argv);		//starts up GLUT
	glutInitDisplayMode(GLUT_RGBA);

	glutInitWindowSize(600, 600);

	glutCreateWindow("SolarSystem");	//creates the window

	glutDisplayFunc(display);	//registers "display" as the display callback function

	init();

	glutMainLoop();				//starts the event glutMainLoop
	return(0);					//return may not be necessary on all compilers
}
