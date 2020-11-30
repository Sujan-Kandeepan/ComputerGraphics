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
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "PPM.h"

GLdouble eye[] = { 0, 3, 10 };
GLdouble lookAt[] = { 0, 0, 0 };
GLdouble up[] = { 0, 1, 0 };

GLfloat ambient[4] = { 0.1, 0.1, 0.1, 1 };
GLfloat diffuse[4] = { 1, 1, 1, 1 };
GLfloat specular[4] = { 0.2, 0.2, 0.2, 1 };
GLfloat lightPos[4] = { -5, 3, 3, 1 };

unsigned int viewportWidth  = 600;
unsigned int viewportHeight = 600;

void drawTeapot(GLfloat size) {
    glFrontFace(GL_CW);
    glutSolidTeapot(size);
    glFrontFace(GL_CCW);
}

struct Teapot {
    float mPos[3];
    float mRotY;
    float mSize;

    Teapot()
    : mPos {0, 1, 0}
    , mRotY(-45)
    , mSize(1.5)
    {}

    void draw() {
        glPushMatrix();
        
            glTranslatef(mPos[0], mPos[1], mPos[2]);
            glRotatef(mRotY, 0, 1, 0);
            drawTeapot(mSize);
        glPopMatrix();
    }
};

struct Image {
    int mWidth;
    int mHeight;
    GLubyte * mImage;

    void load(char * filename) {
        /**
         * YOUR CODE HERE
         */
    }

    void draw(unsigned int x, unsigned int y) {
        glRasterPos2i(x + mWidth, y);
        /**
         * If you are on a retina display then you should replace the values
         * from -1, 1 to -2, 2 to ensure they appear at full size!
         *
         * The PPM parser packs the image mirrored horizontally. Thus we use
         * glPixelZoom to "flip" the image back the correct way by scaling the
         * x axis by -1. This is the same concept as mirroring an object with
         * glScalef(-1, 1, 1).
         *
         * Aside: Using the parsing code from last year. I don't think the code
         * is very good. There is definitely a way to parse PPM bitmaps without
         * needing to flip the image like this.
         */
        glPixelZoom(-1, 1);
        /**
         * YOUR CODE HERE
         */
    }

    void texture() {
        /**
         * YOUR CODE HERE FOR BONUS
         *
         * Add the glTexImage2D and glTexParameterf calls.
         * I strongly recommend reading the documentation to get a loose sense
         * of what these values mean.
         */
    }
};

struct Handler {
    unsigned int mLeft, mRight, mTop, mBottom;
    void (*mHandlerFunc)();

    bool isInBounds(unsigned int x, unsigned int y) {
        /**
         * YOUR CODE HERE, replace `false` with your expression.
         */
        return false;
    }

    void handleClickAt(unsigned int x, unsigned int y) {
        if (isInBounds(x, y)) {
            mHandlerFunc();
        }
    }

    void drawBoxVertices() {
        glVertex3f(mLeft, mTop, 1);
        glVertex3f(mLeft, mBottom, 1);
        glVertex3f(mRight, mTop, 1);
        glVertex3f(mRight, mBottom, 1);
        glVertex3f(mLeft, mTop, 1);
        glVertex3f(mRight, mTop, 1);
        glVertex3f(mLeft, mBottom, 1);
        glVertex3f(mRight, mBottom, 1);
    }
};

struct InteractionHandler {
    std::vector<Handler *> mHandlers;

    void leftClickDown(int x, int y) {
        std::cout << "Left click at " << x << ", " << y << std::endl;
        for (Handler *handler : mHandlers) {
            /**
             * YOUR CODE HERE
             */
        }
    }

    void drawHandlers() {
        glColor3f(1, 1, 1);
        glLineWidth(2);
        glBegin(GL_LINES);
        for (Handler *handler : mHandlers) {
            handler->drawBoxVertices();
        }
        glEnd();
    }

    void addHandler(Handler *handler) {
        /**
         * YOUR CODE HERE
         */
    }
};

Teapot teapot1;
Image hudInterfaceImage;
Image marbleTexture;
InteractionHandler mouseHandler;

void setPerspectiveProjection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1, 1, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/** 3D scene in in here!
 */
void displayPerspective() {
    setPerspectiveProjection();
    gluLookAt(
        eye[0], eye[1], eye[2],
        lookAt[0], lookAt[1], lookAt[2],
        up[0], up[1], up[2]
    );

    /**
     * YOUR CODE HERE FOR BONUS
     *
     * Call texture on marbleTexture to set our texture parameters for the
     * teapot.
     */
    teapot1.draw();
}

void setOrthographicProjection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, viewportWidth, 0, viewportHeight);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/** 2D renderings (like bitmap images) in here!
 */
void displayOrthographic() {
    setOrthographicProjection();
    mouseHandler.drawHandlers();
    /**
     * YOUR CODE HERE
     *
     * draw the hudInterfaceImage instance!
     */
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    displayOrthographic();
    displayPerspective();

    glFlush();
    glutSwapBuffers();
}

void handleReshape(int w, int h) {
    glViewport(0, 0, w, h);
    viewportWidth = w;
    viewportHeight = h;
}

void handleKeyboard(unsigned char key, int _x, int _y) {
    if (key == 'q') {
        exit(0);
    }
}

void handleMouse(int button, int state, int x, int y) {
    /**
     * YOUR CODE HERE
     *
     * Check for GLUT_LEFT_BUTTON and GLUT_DOWN and then call
     * mouseHandler.leftClickDown, remember viewportHeight - y.
     */
}

void fps(int value) {
    glutPostRedisplay();
    glutTimerFunc(17, fps, 0);
}

void moveLeft() {
    teapot1.mPos[0] -= 0.05;
}

void moveRight() {
    teapot1.mPos[0] += 0.05;
}

Handler leftButton = {
    10,
    65,
    100,
    50,
    moveLeft
};

Handler rightButton = {
    110,
    165,
    100,
    50,
    moveRight
};

/**
 * YOUR CODE HERE
 *
 * Add four more functions:
 * moveUp, moveDown, rotateLeft, rotateRight
 */


int main(int argc, char** argv) {
    /** YOUR CODE HERE
     *
     * Load hudInterfaceImage
     */

    /**
     * YOUR CODE HERE
     *
     * Call addHandler for our six handlers.
     */

    /** YOUR CODE HERE FOR BONUS
     *
     * Load marbleTexture
     */
    
    glutInit(&argc, argv);
    glutInitWindowSize(viewportWidth, viewportHeight);
    glutInitWindowPosition(300, 300);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutCreateWindow("Tutorial 6");

    glutKeyboardFunc(handleKeyboard);
    glutMouseFunc(handleMouse);
    glutReshapeFunc(handleReshape);
    glutDisplayFunc(display);

    glEnable(GL_DEPTH_TEST);
    /**
     * Disable lighting temporarily here by commenting these two lines out.
     */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    /** YOUR CODE HERE for bonus
     *
     * Enable textures.
     */

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    glClearColor(1, 1, 1, 1);

    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);

    glutTimerFunc(0, fps, 0);
    glutMainLoop();

    return 0;
}
