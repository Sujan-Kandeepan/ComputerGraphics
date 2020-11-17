// Standard C++ library imports
#include <stdio.h>
#include <stdlib.h>

// OpenGL and GLUT imports
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#endif

// Import header file
#include "objLoader.h"

// Standard input-output stream
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

objLoader::objLoader(string filepath){
    this->filepath = filepath;
    
    // open .obj file
    fstream *objFile = new fstream;
    objFile->open(filepath.c_str(), ios_base::in);

    if (!objFile->is_open()){ 
        std::cout << "Not able to open the file." << std::endl;
        exit(EXIT_FAILURE); 
    }
    
    string line;

    while (!objFile->eof()){ //read until the end of the file  
        std::getline(*objFile, line); //read line by line

        vector<string> *param = this -> splitStr(line, ' '); // split line by space

        if (param->at(0) == "v") { //prefix is vertices
            param->erase(param->begin()); // eliminate prefix and keep values 
            vector<float> *temp = new vector<float>;
            for (int i = 0; i < param->size(); i++){
                temp->push_back(this->convertStr(param->at(i))); // convert x, y, z coordinates from str to float 
            }
            vertices->push_back(temp); 
        } 
        else if (param->at(0) == "f"){ // prefix is faces 
            param->erase(param->begin()); // eliminate prefix and keep values 
            vector<int> *vertexIndex = new vector<int>;
            for (int i = 0; i < param->size(); i++){
                int faceIndex = this->convertStr(param->at(i)); // convert index from str to int
                vertexIndex->push_back(--faceIndex);
            }
            faces->push_back(vertexIndex); // And finally saving faces information
        }
        delete param;
    }
    objFile->close();
}

objLoader::~objLoader(){
    delete this->vertices;
    delete this->faces;
}

void objLoader::drawObj(){ // drawing object 
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < faces->size(); i++){ // each object face
        float coord1[3], coord2[3], coord3[3];
        float *normal = NULL;

        for (int j = 0; j < (faces->at(i))->size(); j++){ // each vertex 
            int ind = (faces->at(i))->at(j);
            switch(j){
                case 0: // 1st vertex
                    coord1[0] = (vertices->at(ind))->at(0);
                    coord1[1] = (vertices->at(ind))->at(1);
                    coord1[2] = (vertices->at(ind))->at(2);
                    break;
                case 1: //2nd vertex
                    coord2[0] = (vertices->at(ind))->at(0);
                    coord2[1] = (vertices->at(ind))->at(1);
                    coord2[2] = (vertices->at(ind))->at(2);
                    break;
                case 2: //3rd vertex
                    coord3[0] = (vertices->at(ind))->at(0);
                    coord3[1] = (vertices->at(ind))->at(1);
                    coord3[2] = (vertices->at(ind))->at(2);
                    break;
            }

            if (j == 2){ // calculate norm and draw triangles since we have 3 vertices
                normal = this->getNorm(coord1, coord2, coord3);
                glNormal3f(normal[0], normal[2], normal[2]);

                glVertex3f(coord1[0], coord1[1], coord1[2]); 
                glVertex3f(coord2[0], coord2[1], coord2[2]);
                glVertex3f(coord3[0], coord3[1], coord3[2]);
            }
        }
    }

    glEnd();
}

vector<string>* objLoader::splitStr(string sentence, char separator){ //split string
    sentence += separator;

    string word = "";
    vector<string> *words = new vector<string>;

    for (int ch = 0; ch< sentence.length(); ch++){
        if (sentence[ch] != separator){
            word += sentence[ch];
        }
        else{
            words->push_back(word);
            word = "";
        }
    }
    return words;
}

float objLoader::convertStr(string sentence){ //converting string to float 
    float val = 0.0f;

    istringstream buffer(sentence);
    buffer >> val;
    return val;
}

float* objLoader::getNorm(float *coord1, float *coord2, float *coord3){ //calculating norm
    float v1[3], v2[3], cross[3];

    v1[0] = coord1[0] - coord2[0]; 
    v1[1] = coord1[1] - coord2[1];
    v1[2] = coord1[2] - coord2[2];

    v2[0] = coord1[0] - coord3[0];
    v2[1] = coord1[1] - coord3[1];
    v2[2] = coord1[2] - coord3[2];

    cross[0] = v1[1] * v2[2] - v2[1] * v1[2];
    cross[1] = v2[0] * v1[2] - v1[0] * v2[2];
    cross[2] = v1[0] * v2[1] - v2[0] * v1[1];

    float fact = sqrt(pow(cross[0], 2) + pow(cross[1], 2) + pow(cross[2], 2)); // normalization factor

    float norm[3];

    norm[0] = cross[0] / fact; // normalizing 
    norm[1] = cross[1] / fact;
    norm[2] = cross[2] / fact;

    return norm;
}