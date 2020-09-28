#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

#include <stdio.h>
#include <stdlib.h>

static GLfloat red[] = { 1.0, 0.0, 0.0 };
static GLfloat green[] = { 0.0, 1.0, 0.0 };
static GLfloat blue[] = { 0.0, 0.0, 1.0 };
static GLfloat white[] = { 1.0, 1.0, 1.0 };

/* display function - GLUT display callback function
 *		clears the screen, draws a square, and displays it
 */
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_TRIANGLES);
		glColor3fv(green);
		glVertex2f(-0.5, -0.5);
		glColor3fv(red);
		glVertex2f(-0.5, 0.5);
		glColor3fv(blue);
		glVertex2f(0.5, 0.5);

		glColor3fv(red);
		glVertex2f(0.5, -0.5);
		glColor3fv(green);
		glVertex2f(1.5, -0.5);
		glColor3fv(blue);
		glVertex2f(0.5, 0.5);

		glColor3fv(red);
		glVertex2f(1.5, 0.5);
		glColor3fv(green);
		glVertex2f(1.5, 1.5);
		glColor3fv(blue);
		glVertex2f(0.5, 0.5);

		glColor3fv(red);
		glVertex2f(0.5, 1.5);
		glColor3fv(green);
		glVertex2f(-0.5, 1.5);
		glColor3fv(blue);
		glVertex2f(0.5, 0.5);
	glEnd();

	glColor3fv(white);
	glTranslatef(0.5, 0.5, 0);
	glutSolidTeapot(0.25);
	glTranslatef(-0.5, -0.5, 0);

	glFlush();

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'q':
		case 27:
			exit(0);
			break;
	}
}

/* main function - program entry point */
int main(int argc, char** argv)
{
	if (argc != 3)
	{
		printf("Error! Must specify two arguments.\n");
		return(1);
	}

	glutInit(&argc, argv);		//starts up GLUT
	glutInitWindowSize(atoi(argv[1]), atoi(argv[2]));
	
	glutCreateWindow("square");	//creates the window
	gluOrtho2D(-1, 2, -1, 2);
	glutDisplayFunc(display);	//registers "display" as the display callback function
	glutKeyboardFunc(keyboard);
	glutMainLoop();				//starts the event loop

	return(0);					//return may not be necessary on all compilers
}
