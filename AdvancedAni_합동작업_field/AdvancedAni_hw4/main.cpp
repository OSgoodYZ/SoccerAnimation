#pragma once

#include <iostream>
#include <string>
#include <vector>

//when linux setting is
//#include </usr/include/GL/gl.h>
//#include </usr/include/GL/glu.h>
//#include </usr/include/GL/glut.h>

//when window setting is
#include <ctime>
//when window setting is

#include <GL/glut.h>


#include "BVHObject.h"
#include "Field.h"
#include "FlyingCam.h"
#include "PBD_Cloth.h"
#include "Ball.h"

using namespace std;



//using class instance
FlyingCam flyCam;
BVHObject bvhObject;
Field field;
Ball ball;
PBD_Cloth GoalNet0(40,8,0.25);
PBD_Cloth GoalNet1(40,16,0.25);
PBD_Cloth GoalNet2(8, 16, 0.25);
PBD_Cloth GoalNet3(8, 16, 0.25);
vector<PBD_Cloth*> GoalNetSet;


//clock_t   timeRecorder;

//OpenGL system setting
int       width = 1024;
int       height = 768;
int option = -1;

//Surrounding Setting
const GLfloat  lightPosition[4] = { -5.0f, 10.0f, 10.0f, 1.0f };
const float    wallSize = 40.0f;
const float    modelScale = 0.020f;

//Frame & TimeStep Setting	[MODEL]
int      ModelFrameNumber = 0;					

//Frame & TimeStep Setting	[CLOTH]
float	 timeStep = 1.0f / 60.0f; //1.0/60.0f;
//float	 currentTime = 0;
//double	 accumulator = timeStep;
//float	 startTime = 0, fps = 0;


//Model Setting
string   fileName;


//##################################				LCJ WORKING			###########################################
void ChangeSize(GLsizei w, GLsizei h)
{

	glViewport(0, 0, w, h);

	width = w;
	height = h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80, width / height, 1, 40000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void processMouse(int button, int state, int x, int y) {
	flyCam.FlyingCamCursorUpdate(button, state, x, y);
}
void drag_func(int x, int y)
{

	flyCam.FlyingCamAngleDisUpdate(x, y);
	glutPostRedisplay();
}

void scene(void) {
	// wall
	const float size = wallSize;
	const float d = size / 10.0f;
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	for (double dx0 = -size; dx0 < size; dx0 += d) {
		for (double dy0 = -size; dy0 < size; dy0 += d) {
			double dx1 = dx0 + d, dy1 = dy0 + d;
			glVertex3f(dx0, 0.0, dy0);
			glVertex3f(dx0, 0.0, dy1);
			glVertex3f(dx1, 0.0, dy1);
			glVertex3f(dx1, 0.0, dy0);
		}
	}
	glEnd();

	glDisable(GL_LIGHTING);

	field.render();
	glEnable(GL_LIGHTING);


	// character
	if (bvhObject.ready) {
		const float scale = modelScale;
		glColor3f(1.0f, 0.0f, 0.0f);
		glScalef(scale, scale, scale);
		bvhObject.render(ModelFrameNumber);

		ModelFrameNumber = ModelFrameNumber + 2;
		ModelFrameNumber %= bvhObject.nFrames;
		cout << "ModelFrameNumber:	" << ModelFrameNumber << endl;
	}
}
void display()
{
	//##################################				LCJ WORKING			###########################################
	//#########		LCJ FRAME ############
	//float newTime = (float)glutGet(GLUT_ELAPSED_TIME);
	//frameTime = newTime - currentTime;
	//currentTime = newTime;
	//#########		LCJ FRAME ############

	//glMatrixMode(GL_MODELVIEW);
	//이 부분 카메라 클래스안으로 넣을 필요있음.
	//카메라 class 안에 fov추가해서 구현하기
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80, width / height, 1, 40000);

	//camera
	glTranslatef(0, 0, flyCam.dist);
	glRotatef(flyCam.rX, 1, 0, 0);
	glRotatef(flyCam.rY, 0, 1, 0);
	//glColor3f(1.0, 0.2, 0.5);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//##################################				LCJ WORKING			###########################################


	glColor3f(0.4, 0.4, 0.4);

	scene();

	for (vector<PBD_Cloth*>::iterator it = GoalNetSet.begin(); it != GoalNetSet.end(); ++it)
	{
		(*it)->DrawCloth();;
	}

	ball.render();

	glutSwapBuffers();
	//glutPostRedisplay();
}



void glut_idle(void) {
	//clock_t targetInterval = static_cast<clock_t>(1000.0 * 1000.0 * bvhObject.interval);
	//if (clock() - timeRecorder > targetInterval) {
	//    //glutPostRedisplay();
	//    timeRecorder = clock();

	//}
	//cout << "timeRecorder " << timeRecorder << endl;

	//############# LCJ CLOTH ###############
	for (vector<PBD_Cloth*>::iterator it = GoalNetSet.begin(); it != GoalNetSet.end(); ++it)
	{
		(*it)->StepPhysics(timeStep);
	}


	glutPostRedisplay();
	//Sleep(5); //TODO
}


void menu(int id)
{
	option = id;
	switch (option)
	{
	case 0:
		fileName = string("BVH_Data/1001.bvh");
		bvhObject.init(fileName);
		break;
	case 1:
		fileName = string("BVH_Data/1002.bvh");
		bvhObject.init(fileName);
		break;
	case 2:
		fileName = string("BVH_Data/1003.bvh");
		bvhObject.init(fileName);
		break;
	case 3:
		fileName = string("BVH_Data/1005.bvh");
		bvhObject.init(fileName);
		break;
	case 4:
		fileName = string("BVH_Data/1006.bvh");
		bvhObject.init(fileName);
		break;
	case 5:
		fileName = string("BVH_Data/1101.bvh");
		bvhObject.init(fileName);
		break;

	}

}

void keyboardCB(unsigned char keyPressed, int x, int y)
{
	switch (keyPressed) {
	case 'q':
		exit(0);
		break;
	}
	glutPostRedisplay();
}
void init() {
	GoalNet0.initialization(topSide);
	GoalNetSet.push_back(&GoalNet0);
	GoalNet1.initialization(backSide);
	GoalNetSet.push_back(&GoalNet1);
	GoalNet2.initialization(leftSide);
	GoalNetSet.push_back(&GoalNet2);
	GoalNet3.initialization(rightSide);
	GoalNetSet.push_back(&GoalNet3);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glDisable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_DIFFUSE);

	const GLfloat lightColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const GLfloat lightAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f };

	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);

	const GLfloat ambient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	const GLfloat specular[] = { 0.4f, 1.0f, 0.4f, 1.0f };
	const GLfloat shininess[] = { 20.0f };

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}
void manual()
{
	std::cout << "==================manual=================" << std::endl;
	std::cout << std::endl;
	std::cout << "   START   :  right click and select bvh" << std::endl;
	std::cout << "   EXIT    :  press 'q' key " << std::endl;
	std::cout << "  Rotate   :  left click & drag" << std::endl;
	std::cout << "   Zoom    :  ctrl + left click & drag" << std::endl;
	std::cout << " Translate :  middle click & drag" << std::endl;

	std::cout << std::endl;
	std::cout << "=========================================" << std::endl;
}

int main(int argc, const char **argv) {

	glutInit(&argc, const_cast<char **>(argv));
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Final Project [Soccer penalty shoot-out]");
	manual();



	glutDisplayFunc(display);
	glutReshapeFunc(ChangeSize);
	glutIdleFunc(glut_idle);

	glutKeyboardFunc(keyboardCB);
	glutMouseFunc(processMouse);
	glutMotionFunc(drag_func);

	glutCreateMenu(menu);
	glutAddMenuEntry("KickMotion 1", 0);
	glutAddMenuEntry("KickMotion 2 ", 1);
	glutAddMenuEntry("KickMotion 3 ", 2);
	glutAddMenuEntry("KickMotion 4 ", 3);
	glutAddMenuEntry("KickMotion 5 ", 4);
	glutAddMenuEntry("KickMotion 6 ", 5);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	//glewInit();
	init();

	glutMainLoop();

	return 0;
}
