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


using namespace std;



//using class instance
FlyingCam flyCam;
BVHObject bvhObject;
BVHObject2 bvhObject2;
Field field;
Ball ball;
PBD_Cloth GoalNet0(40,8,0.25);
PBD_Cloth GoalNet1(40,16,0.25);
PBD_Cloth GoalNet2(8, 16, 0.25);
PBD_Cloth GoalNet3(8, 16, 0.25);
vector<PBD_Cloth*> GoalNetSet;
clock_t ani_start, ani_time;
double  animation_time = 0.0f;
double  update_timer = 0.0f;
double  update_timer2 = 0.0f;
double time_scale = 1.0f;
double time_scale2 = 1.0f;

//clock_t   timeRecorder;

//OpenGL system setting
int       width = 1024;
int       height = 768;
int option = -1;

//Surrounding Setting
//const GLfloat  lightPosition[4] = { -5.0f, 10.0f, 10.0f, 1.0f };
const float    wallSize = 40.0f;
float    modelScale = 0.015f;
float    modelScale2 = 2.300f;

extern int motion_kicker;
extern int motion_keeper;

//Frame & TimeStep Setting	[MODEL]
int      ModelFrameNumber = 0;					
int      ModelFrameNumber2 = 0;
//Frame & TimeStep Setting	[CLOTH]
float	 timeStep = 1.0f / 60.0f; //1.0/60.0f;
//float	 currentTime = 0;
//double	 accumulator = timeStep;
//float	 startTime = 0, fps = 0;


//Model Setting
string   fileName;
string   fileName2;

// display text 
static void drawString(void* font, const char* str, float x, float y) {
	int len = strlen(str);
	glRasterPos2f(x, y);
	for (int i = 0; i < len; ++i) {
		glutBitmapCharacter(font, str[i]);  // this function automatically
											// advances raster position
	}
}

// display text 
static void drawMessage(int line_no, char messages[][64], int num_messages)
{
	if (messages == NULL || num_messages <= 0)
		return;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, 500, 500, 0.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glColor3f(0.0, 1.0, 0.0);
	int y = 24 + 18 * line_no;
	for (int i = 0; i < num_messages; ++i) {
		drawString(GLUT_BITMAP_HELVETICA_18, messages[i], 8, y);
		y += 18;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}



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


	
	//glEnable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	field.render();
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glDisable(GL_LIGHT0);


	// character(Kicker)
	if (bvhObject.ready) {
		float scale = modelScale;
		glLoadIdentity();
		glColor3f(1.0f, 0.0f, 0.0f);
		glScalef(scale, scale, scale);
		bvhObject.render(ModelFrameNumber);
	}

	// character (Keeper)
	if (bvhObject2.ready) {
		float scale2 = modelScale2;
		glLoadIdentity();
		glColor3f(1.0f, 0.0f, 0.0f);
		glScalef(scale2, scale2, scale2);
		bvhObject2.render(ModelFrameNumber2);
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

	// display text
	char messages[1][64];
	if (bvhObject.ready) {
		sprintf(messages[0], "Shoot :  's' key");
		drawMessage(0, messages, 1);
	}
	else {
		sprintf(messages[0], "Start : 'i' key");
		drawMessage(0, messages, 1);
	}

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

	//	LCJ 충돌실험
	//Vector3f veltemp(0, 0.2, 4);
	//ball.getPosition();
	//ball.setPosition(ball.getPosition() + veltemp * timeStep);

	//#############				공의 충돌전 움직임			############################
	//ball.setVel(Vector3f (0, 0.2, 4));
	
	ball.tmp_position = ball.position;
	ball.setNewVel(ball.velocity + timeStep*(ball.gravity*100 + ball.forces *(1/ball.mass)));
	//cout << "newvel" << ball.newVelocity <<endl;
	ball.position = ball.tmp_position +ball.newVelocity*timeStep;
	ball.tmp_position = ball.position;
	//#############				공의 충돌전 움직임			############################
	vector <float> collisionCoefficiet;
	float sumCollCoeffi = 0.0f;
	//############# LCJ CLOTH ###############
	for (vector<PBD_Cloth*>::iterator it = GoalNetSet.begin(); it != GoalNetSet.end(); ++it)
	{
		collisionCoefficiet.push_back((*it)->StepPhysics(timeStep, ball));
		
	}

	while (collisionCoefficiet.empty() != true)
	{
		sumCollCoeffi = sumCollCoeffi + collisionCoefficiet[collisionCoefficiet.size() - 1];
		collisionCoefficiet.pop_back();
	}
	cout << "sumCollCoeffi	"<<sumCollCoeffi << endl;
	//#############				공의 충돌후 움직임			############################
	if (sumCollCoeffi != 0) //충돌이 있으면
	{
		ball.newVelocity[0] = 0;// ball.newVelocity[0] - ((0.1)* sumCollCoeffi)*ball.newVelocity[0];
		ball.newVelocity[1] = ball.velocity[1] + timeStep * (ball.gravity[1] * 100 );//ball.newVelocity[0] -((0.05)* sumCollCoeffi)*ball.newVelocity[1];
		ball.newVelocity[2] = 0;// ball.newVelocity[0] - ((0.03)* sumCollCoeffi)*ball.newVelocity[2];
		ball.position = ball.tmp_position + ball.newVelocity*timeStep;


		//속도반대로 바꾸기
		//정확히는 damping먹이기

	}
	//		ground collision
	
	if (ball.position[1] - ball.radius <= 0 ) //collision with ground  axis y
	{
		ball.position[1] = ball.radius;
		ball.newVelocity[1] = -0.5*ball.newVelocity[1];

	}
	if (ball.position[1] + ball.radius >= 4) //collision with 윗그물
	{
		//ball.position[1] = ball.radius;
		ball.newVelocity[1] = 0;

	}
		//tmp_pos[i][1] = 0;
	

	ball.tmp_position = ball.position;
	ball.velocity = ball.newVelocity;
	ball.forces = Vector3f(0, 0, 0);
	//#############				공의 충돌후 움직임			############################

	glutPostRedisplay();
	//Sleep(5); //TODO

		//############# JJH BVH motion ###############
	ani_time = clock();
	float last_time = animation_time;
	animation_time = (ani_time - ani_start) / (float)CLOCKS_PER_SEC;
	float dt = animation_time - last_time;

	if (bvhObject.ready) {
		update_timer += time_scale * 2 * dt;
		if ((ModelFrameNumber + 1) >= bvhObject.nFrames) {
			if (motion_kicker == 0)
			{
				bvhObject.pose_save1(ModelFrameNumber);
				fileName = string("BVH_Data/kicker/1001_nohand_shoot.bvh");
				ModelFrameNumber = 0;
				motion_kicker = 1;
				time_scale = 1.5f; //bvh speed
				bvhObject.init(fileName);
				bvhObject.pose_save2(0);
			}
			else if (motion_kicker == 1)
			{
				bvhObject.pose_save1(ModelFrameNumber);
				fileName = string("BVH_Data/kicker/laugh.bvh");
				ModelFrameNumber = 0;
				motion_kicker = 2;
				time_scale = 1.8f; //bvh speed
				bvhObject.init(fileName);
				bvhObject.pose_save2(0);
			}
		}

		else if (update_timer > bvhObject.interval) {
			ModelFrameNumber++;
			update_timer = 0;
		}
	}
	else {
		ModelFrameNumber = 0;
	}

	if (bvhObject2.ready) {
		update_timer2 += time_scale2 *10.0* dt;
		if ((ModelFrameNumber2 + 1) >= bvhObject2.nFrames) {
			if (motion_keeper == 0)
			{
				bvhObject2.pose_save1(ModelFrameNumber2);
				fileName2 = string("BVH_Data/keeper/keeper_blocking_reverse_short.bvh");
				ModelFrameNumber2 = 0;
				motion_keeper = 1;
				time_scale2 = 2.0f; //bvh speed
				bvhObject2.init(fileName2);
				bvhObject2.pose_save2(0);

			}
			else if (motion_keeper == 1)
			{
				bvhObject2.pose_save1(ModelFrameNumber2);
				fileName2 = string("BVH_Data/keeper/keeper_ready2.bvh");
				ModelFrameNumber2 = 0;
				motion_keeper = 2;
				time_scale2 = 1.8f; //bvh speed
				bvhObject2.init(fileName2);
				bvhObject2.pose_save2(0);
			}
		}

		else if (update_timer2 > bvhObject2.interval) {
			ModelFrameNumber2++;
			update_timer2 = 0;
		}
	}
	else {
		ModelFrameNumber2 = 0;
	}
}



void keyboardCB(unsigned char keyPressed, int x, int y)
{
	switch (keyPressed) {
	case 'q':
		exit(0);
		break;
	case 'i':
		fileName = string("BVH_Data/kicker/1001_nohand_ready.bvh"); //kicker
		ModelFrameNumber = 0;
		//motion_kicker = 0;
		modelScale = 0.02f;
		bvhObject.init(fileName);
		time_scale = 0.6f; //bvh speed
		bvhObject.pose_save2(0);
		bvhObject.pose_save1(0);

		fileName2 = string("BVH_Data/keeper/keeper_ready.bvh"); //keeper
		ModelFrameNumber2 = 0;
		modelScale2 = 0.04f;
		time_scale2 = 2.0f;//1.8f; //bvh speed

		bvhObject2.init(fileName2);
		bvhObject2.pose_save2(0);
		bvhObject2.pose_save1(0);

		break;
	case 's':
		motion_kicker = 0;
		motion_keeper = 0;

		Vector3f tmpforce(5000, 1000, 20000);
		ball.forces = tmpforce;
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

	const GLfloat lightColor[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	const GLfloat lightAmbient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float Position[] = { 0.0f, 1000.0f, 10.0f, 1.0f };  //조명 위치

	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);
	glLightfv(GL_LIGHT0, GL_POSITION, Position);     // 광원 위치 설정

	const GLfloat ambient[] = { 0.1f, 0.1f, 0.1f, 0.0f };
	const GLfloat specular[] = { 0.5f, 0.5f, 0.5f, 0.0f };
	float diffuse[] = { 0.5f, 0.5f, 0.5f, 0.0f };
	const GLfloat shininess[] = { 60.0f };

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);

	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}
void manual()
{
	std::cout << "==================manual=================" << std::endl;
	std::cout << std::endl;
	std::cout << "   START   :  i key" << std::endl;
	std::cout << "   EXIT    :  press 'q' key " << std::endl;
	std::cout << "  Rotate   :  left click & drag" << std::endl;
	std::cout << "   Zoom    :  wheel click & drag" << std::endl;
	//std::cout << " Translate :  middle click & drag" << std::endl;

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


	//glewInit();
	init();

	glutMainLoop();

	return 0;
}
