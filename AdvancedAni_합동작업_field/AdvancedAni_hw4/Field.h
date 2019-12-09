#pragma once


#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

#include <iostream>
#include <string>
#include <vector>
//#include "Point.h"

class Field {
public:
	Field();

    void render();

    float getWidth();
    float getLenght();

    //Point** getBoundaries();
private:
    float width;
    float lenght;

    void renderGround();
    void renderLines();
    void renderGoal();
};