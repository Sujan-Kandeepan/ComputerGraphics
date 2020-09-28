#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif
#include <iostream>


void drawPoint(int x, int y){
	glPointSize(10);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POINTS);
        glVertex2f(x, y);
	glEnd();

	glutPostRedisplay();
}

void init() {  
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(800, 600);
	glutCreateWindow("3GC3 Lab 2");
	glClearColor(0.0,0.0,0.0,0);
	glColor3f(1.0,1.0,1.0);
    
	gluOrtho2D(0,800,0,600);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	drawPoint(pointX, pointY);
}

/*Main*/
int main (int argc,char** argv){
	glutInit(&argc,argv);
	init();
	glutDisplayFunc(display);
	/* Write your call backs here */
	glutMainLoop();
} 

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif
#include <iostream>
using namespace std;

float animateDistance = 0;
float pointX = -999;
float pointY = -999;

/*This function will require modifications*/
void drawPoint(int x, int y){
	glPointSize(10);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POINTS);
	glVertex2f(x, y);
	glEnd();
}

void init() {  
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(800, 600);
	glutCreateWindow("3GC3 Lab 2");
	glClearColor(0.0,0.0,0.0,0);
	glColor3f(1.0,1.0,1.0);
	gluOrtho2D(0,800,0,600);
}

/*This function will require modifications*/
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
}

/*Main*/
int main (int argc,char** argv){
	glutInit(&argc,argv);
	init();
	glutDisplayFunc(display);
	/* Write your call backs here */
	glutMainLoop();
} 
