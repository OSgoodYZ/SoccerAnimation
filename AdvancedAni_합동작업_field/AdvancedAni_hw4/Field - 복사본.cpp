#include "Field.h"

float M_PI = 3.14;

Field::Field() {
    width = 200;
}

void Field::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderGround();
    renderLines();
    renderGoal();
}

float Field::getWidth() {
    return width;
}

float Field::getLenght() {
    return width / 2;
}

void Field::renderGround() {
    glColor3f(0.14, 0.34, 0.02);
    glBegin(GL_QUADS);
        glVertex3f(-width / 2, 0, -width / 4);
        glVertex3f(-width / 2, 0,  width / 4);
        glVertex3f( width / 2, 0,  width / 4);
        glVertex3f( width / 2, 0, -width / 4);

    glEnd();
}

void Field::renderLines() {
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(2);

    glColor3f(1, 1, 1);

    glBegin(GL_LINE_LOOP); // outline
        glVertex3f(-width / 2 + 3, 0.01, -width / 4 + 3);
        glVertex3f( width / 2 - 3, 0.01, -width / 4 + 3);
        glVertex3f( width / 2 - 3, 0.01,  width / 4 - 3);
        glVertex3f(-width / 2 + 3, 0.01,  width / 4 - 3);
        glVertex3f(-width / 2 + 3, 0.01, -width / 4 + 3);
    glEnd();

    glBegin(GL_LINE_LOOP); // center line
        glVertex3f(0, 0.01, -width / 4 + 3);
        glVertex3f(0, 0.01,  width / 4 - 3);
    glEnd();

    glBegin(GL_LINE_LOOP); //center circle
        for (unsigned int i = 0; i < 360; i++) {
            float angle = i * M_PI / 180;
            glVertex3f(cos(angle) * width/15, 0.01, sin(angle) * width / 15); // diameter
        }
    glEnd();

    glBegin(GL_LINE_LOOP); // penalty line (Opposite side)
        glVertex3f(-width / 2 + 3, 0.01, -width / 7);
        glVertex3f(-width / 2 + width / 7,  0.01, -width / 7);
        glVertex3f(-width / 2 + width / 7,  0.01, width / 7);
        glVertex3f(-width / 2 + 3, 0.01, width / 7);
    glEnd();

    glBegin(GL_LINE_LOOP); //G yard box (Opposite side)
        glVertex3f(-width / 2 + 3, 0.01, -width / 15);
        glVertex3f(-width / 2 + width / 20,   0.01, -width / 15);
        glVertex3f(-width / 2 + width / 20,   0.01, width / 15);
        glVertex3f(-width / 2 + 3, 0.01, width / 15);
    glEnd();

    glBegin(GL_LINE_LOOP); //G yard box 
        glVertex3f(width / 2 - 3, 0.01, -width/15); // -13
        glVertex3f(width / 2 - width / 20,   0.01, -width / 15); //15, -13
        glVertex3f(width / 2 - width / 20,   0.01, width / 15); //15, 13
        glVertex3f(width / 2 -3,  0.01, width / 15); // 13
    glEnd();

    glBegin(GL_LINE_LOOP); // penalty line 
        glVertex3f(width / 2 - 3, 0.01, -width/7);
        glVertex3f(width / 2 - width / 7,  0.01, -width / 7); // -15, -13
        glVertex3f(width / 2 - width / 7,  0.01, width / 7);
        glVertex3f(width / 2 - 3, 0.01, width / 7); // 13
    glEnd();

    glBegin(GL_LINE_LOOP); // penalty line circle
        for (unsigned int i = 135; i < 226; i++) {
            float angle = i * M_PI / 180;
            glVertex3f(cos(angle) * width / 15 + width / 2 - width / 10.45, 0.01, sin(angle) * width / 15);
        }
    glEnd();

    glBegin(GL_LINE_LOOP); // penalty line circle (Opposite side)
        for (unsigned int i = 315; i < 406; i++) {
            float angle = i * M_PI / 180;
            glVertex3f(cos(angle) * width / 15 - width / 2 + width / 10.45, 0.01, sin(angle) * width / 15);
        }
    glEnd();
}

void Field::renderGoal() {
    glPushMatrix();
        glTranslatef(width / 2 - 1.5, 0, -5);
        glRotatef(-90, 1, 0, 0);
        GLUquadricObj* quadObj = gluNewQuadric();
        gluCylinder(quadObj, 0.1, 0.1, 3.5, 10, 10);

        glTranslatef(0, -10, 0);
        gluCylinder(quadObj, 0.1, 0.1, 3.5, 10, 10);

        glTranslatef(0, 0, 3.4);
        glRotatef(-90, 1, 0, 0);
        gluCylinder(quadObj, 0.1, 0.1, 10, 10, 10);
    glPopMatrix();
}

//Point** Field::getBoundaries() {
//    Point** boundaries = (Point**) malloc(sizeof(Point*) * 2);
//
//    boundaries[0] = new Point(-width / 2, 0, -width / 4);
//    boundaries[1] = new Point( width / 2, 0,  width / 4);
//
//    return boundaries;
//}
