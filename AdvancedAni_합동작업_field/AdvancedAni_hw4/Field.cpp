#include "Field.h"

float M_PI = 3.14;

Field::Field() {
    width = 200;
}
int move = -80; // offset of field and model
int outline_offset = 4;
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
        glVertex3f(-width / 4, 0, -width / 2+ move);
        glVertex3f(-width / 4, 0,  width / 2 + move);
        glVertex3f( width / 4, 0,  width / 2 + move);
        glVertex3f( width / 4, 0, -width / 2 + move);

    glEnd();
}

void Field::renderLines() {
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(2);

    glColor3f(1, 1, 1);

    glBegin(GL_LINE_LOOP); // outline
        glVertex3f(-width / 4 + outline_offset, 0.01, -width / 2 + outline_offset + move);
        glVertex3f( width / 4 - outline_offset, 0.01, -width / 2 + outline_offset + move);
        glVertex3f( width / 4 - outline_offset, 0.01,  width / 2 - outline_offset + move);
        glVertex3f(-width / 4 + outline_offset, 0.01,  width / 2 - outline_offset + move);
        glVertex3f(-width / 4 + outline_offset, 0.01, -width / 2 + outline_offset + move);
    glEnd();

    glBegin(GL_LINE_LOOP); // center line
        glVertex3f(-width / 4 + outline_offset, 0.01, 0 + move);
        glVertex3f(width / 4 - outline_offset, 0.01,  0 + move);
    glEnd();

    glBegin(GL_LINE_LOOP); //center circle
        for (unsigned int i = 0; i < 360; i++) {
            float angle = i * M_PI / 180;
            glVertex3f(sin(angle) * width/15, 0.01, cos(angle) * width / 15 + move);
        }
    glEnd();

    glBegin(GL_LINE_LOOP); // penalty line (Opposite side)
        glVertex3f(-width / 7, 0.01, -width / 2 + outline_offset + move);
        glVertex3f(-width / 7,  0.01, -width / 2 + width / 7 + move);
        glVertex3f(width / 7,  0.01, -width / 2 + width / 7 + move);
        glVertex3f(width / 7, 0.01, -width / 2 + outline_offset + move);
    glEnd();

    glBegin(GL_LINE_LOOP); //G yard box (Opposite side)
        glVertex3f(-width / 15, 0.01, -width / 2 + outline_offset + move);
        glVertex3f(-width / 15,   0.01, -width / 2 + width / 20 + move);
        glVertex3f(width / 15,   0.01, -width / 2 + width / 20 + move);
        glVertex3f(width / 15, 0.01, -width / 2 + outline_offset + move);
    glEnd();

    glBegin(GL_LINE_LOOP); //G yard box 
        glVertex3f(-width / 15, 0.01, width / 2 - outline_offset + move);
        glVertex3f(-width / 15,   0.01, width / 2 - width / 20 + move);
        glVertex3f(width / 15,   0.01, width / 2 - width / 20 + move);
        glVertex3f(width / 15,  0.01, width / 2 - outline_offset + move);
    glEnd();

    glBegin(GL_LINE_LOOP); // penalty line 
        glVertex3f(-width / 7, 0.01, width / 2 - outline_offset + move);
        glVertex3f(-width / 7,  0.01, width / 2 - width / 7 + move);
        glVertex3f(width / 7,  0.01, width / 2 - width / 7 + move);
        glVertex3f(width / 7, 0.01, width / 2 - outline_offset + move);
    glEnd();

    glBegin(GL_LINE_LOOP); // penalty line circle
        for (unsigned int i = 135; i < 226; i++) {
            float angle = i * M_PI / 180;
            glVertex3f(sin(angle) * width / 15, 0.01, cos(angle) * width / 15 + width / 2 - width / 10.45 + move);
        }
    glEnd();

    glBegin(GL_LINE_LOOP); // penalty line circle (Opposite side)
        for (unsigned int i = 315; i < 406; i++) {
            float angle = i * M_PI / 180;
            glVertex3f(sin(angle) * width / 15, 0.01, cos(angle) * width / 15 - width / 2 + width / 10.45 + move);
        } 
    glEnd();
}

void Field::renderGoal() {
    glPushMatrix();
        glTranslatef(-5, 0, width / 2 - outline_offset + move);
        glRotatef(-90, 1, 0, 0); 
        GLUquadricObj* quadObj = gluNewQuadric();
        gluCylinder(quadObj, 0.1, 0.1, 4, 10, 10);

        glTranslatef(10, 0, 0);  
        gluCylinder(quadObj, 0.1, 0.1, 4, 10, 10);

        glTranslatef(0, 0, 4);
        glRotatef(-90, 1, 0, 0); 
		glRotatef(-90, 0, 1, 0); 
        gluCylinder(quadObj, 0.1, 0.1, 10, 10, 10);  

		glTranslatef(0, 0, 0);
		glRotatef(-90, 0, 1, 0); 
		gluCylinder(quadObj, 0.1, 0.1, 2, 10, 10); 

		glTranslatef(0, 0, 2);
		glRotatef(-70, 1, 0, 0); 
		gluCylinder(quadObj, 0.1, 0.1, 4.2, 10, 10);  

		
		glTranslatef(0, 0, 4.2);
		glRotatef(90, 0, 1, 0);   
		//glLoadIdentity()

		glTranslatef(0, 0, 10);
		glRotatef(90, 0, 1, 0); 
		gluCylinder(quadObj, 0.1, 0.1, 4.2, 10, 10);

		glTranslatef(0, 0, 4.2);
		glRotatef(-70, 1, 0, 0); 
		gluCylinder(quadObj, 0.1, 0.1, 2, 10, 10);

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
