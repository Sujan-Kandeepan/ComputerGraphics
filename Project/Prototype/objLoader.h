#ifndef OBJLOADER_H
#define OBJLOADER_H

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#endif

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>

using namespace std;

class objLoader{
    public:
        objLoader(string filepath); // filepath of .obj file
        ~objLoader(); // destructor
        void drawObj(); // drawing  

    private:
        string filepath = "";
        vector<vector<float>*> *vertices = new vector<vector<float>*>;
        vector<vector<int>*>   *faces    = new vector<vector<int>*>;

        vector<string>* splitStr(string sentence, char separator); //split string given by a separator character

        float convertStr(string sentence); //converting string value to float

        float* getNorm(float *coord1, float *coord2, float *coord3); //calculates normal 
};

#endif 