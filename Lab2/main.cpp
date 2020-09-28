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
#include <stdio.h>

int pointX = 0, pointY = 0, offset = 0, pause = 0,
	rainbow = 0, width = 800, height = 600;

float colours[5][3];

void drawPoint(int x, int y){
	glPointSize(10);
	glBegin(GL_POINTS);
        glVertex2f(x, y);
	glEnd();
	glFlush();
	glutPostRedisplay();
}

void init() {  
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(width, height);
	glutCreateWindow("3GC3 Lab 2");
	glClearColor(0.0,0.0,0.0,0);
	glColor3f(1.0,1.0,1.0);
    
	gluOrtho2D(0,width,0,height);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3fv(colours[0]);
	drawPoint(pointX, pointY);
	glColor3fv(colours[1]);
	drawPoint(pointX + offset, pointY);
	glColor3fv(colours[2]);
	drawPoint(pointX - offset, pointY);
	glColor3fv(colours[3]);
	drawPoint(pointX, pointY + offset);
	glColor3fv(colours[4]);
	drawPoint(pointX, pointY - offset);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'p':
		case 'P':
			if (pause == 0)
				pause = 1;
			else if (pause == 1)
				pause = 0;
			break;
		case 'q':
		case 'Q':
			exit(0);
			break;
		case ' ':
			if (rainbow == 0)
				rainbow = 1;
			else if (rainbow == 1)
				rainbow = 0;
			break;
		break;
	}
}

void mouse(int button, int state, int x, int y)
{
	y = height - y;

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (rainbow == 1)
					colours[i][j] = ((float) rand()) / (float) RAND_MAX;
				else
					colours[i][j] = 1.0f;
			}
		}
		printf("%.3f %.3f %.3f\n", colours[0][0], colours[0][1], colours[0][2]);

		pause = 0;
		pointX = x;
		pointY = y;
		offset = 0;
	}
}
void timer(int val)
{
	if (pause == 0	)
		offset += 5;
	glutTimerFunc(20, timer, 0);
}

/*Main*/
int main (int argc,char** argv){
	glutInit(&argc,argv);
	init();
	glutDisplayFunc(display);
	/* Write your call backs here */
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutTimerFunc(20, timer, 0);
	glutMainLoop();
} 
