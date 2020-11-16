#ifdef __APPLE__
#  define GL_SILENCE_DEPRECATION
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
#include <math.h>

GLdouble eye[] = { -5, 5, 10 };
GLdouble lookAt[] = { 0, 0, 0 };
GLdouble up[] = { 0, 1, 0 };

GLfloat ambient[4] = { 0.1, 0.1, 0.1, 1 };
GLfloat diffuse[4] = { 1, 1, 1, 1 };
GLfloat specular[4] = { 0.2, 0.2, 0.2, 1 };
GLfloat lightPos[4] = { -5, 0, 3, 1 };

GLfloat materialAmbient[4] =  { 0, 0.3, 0, 1.0 };
GLfloat materialDiffuse[4] =  { 0.2, 0.4, 0.2, 1.0 };
GLfloat materialSpecular[4] = { 0.3, 0.3, 0.3, 1.0 };
GLfloat materialShiny = 20;

GLfloat outlineAmbient[4] =  { 0.0, 0.0, 0.0, 1.0 };
GLfloat outlineDiffuse[4] =  { 0.0, 0.0, 0.0, 1.0 };
GLfloat outlineSpecular[4] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat outlineShiny = 0;

void setMaterials() {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialShiny);
}

void setOutlineMaterials() {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, outlineAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, outlineDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, outlineSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, outlineShiny);
}

GLfloat cubeVertices[8][3] = {
    { -1, 1, -1 },
    { -1, -1, -1 },
    { 1, -1, -1 },
    { 1, 1, -1 },


    { -1, 1, 1 },
    { -1, -1, 1 },
    { 1, -1, 1 },
    { 1, 1, 1 },
};

unsigned int cubeTriangles[12][3] = {
    // Back face
    { 0, 2, 1 },
    { 0, 3, 2 },

    // Left face
    { 0, 1, 4 },
    { 1, 5, 4 },

    // Front face
    { 4, 5, 6 },
    { 4, 6, 7 },

    // Right face
    { 6, 3, 7 },
    { 2, 3, 6 },

    // Top face
    { 0, 4, 3 },
    { 3, 4, 7 },

    // Bottom face
    { 1, 2, 5 },
    { 2, 6, 5 }
};

// Define cube normals here, should be in similar format as cubeTriangles above

void drawCube() {
    glFrontFace(GL_CCW);

    glBegin(GL_TRIANGLES);
    // Render each triangle
    for (int i = 0; i < 12; i++) {
        // Specify the normal for this triangle.
        // We only need six normals since each face of the cube is composed of
        // two triangles.
		// insert code here!
        // Render the three vertices of each triangle
        for (int j = 0; j < 3; j++) {
            glVertex3fv(cubeVertices[cubeTriangles[i][j]]);
        }
    }
    glEnd();
}

void drawTeapot(GLfloat size) {
    glFrontFace(GL_CW);
    glutSolidTeapot(size);
    glFrontFace(GL_CCW);
}

void display(void) {
    glClearColor(0.7, 0.7, 0.7, 1);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(
        eye[0], eye[1], eye[2],
        lookAt[0], lookAt[1], lookAt[2],
        up[0], up[1], up[2]
    );

    glPushMatrix();
        glTranslatef(0, 2, 0);
		// insert teapot + culling init here
        setMaterials();
        drawTeapot(1);
    glPopMatrix();


   setMaterials();
   drawCube();
	// insert cube + transformation + culling init here
    glFlush();
}

void handleReshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1, 1, 100);

    glMatrixMode(GL_MODELVIEW);
}

void handleKeyboard(unsigned char key, int _x, int _y) {
    if (key == 'q') {
        exit(0);
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(600,600);
    glutInitWindowPosition(300,300);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Tutorial 5");

    glutKeyboardFunc(handleKeyboard);
    glutReshapeFunc(handleReshape);
    glutDisplayFunc(display);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    glFrontFace(GL_CCW);
	// insert culling setup here

    glShadeModel(GL_SMOOTH);

    glutMainLoop();

    return 0;
}
