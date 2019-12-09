#pragma once

#include <GL/glut.h>
#include "Vector3.h"

class FlyingCam
{
public:
	FlyingCam();
	~FlyingCam();
	void FlyingCamCursorUpdate(int mousebutton, int mousestate, int mouseX, int mouseY);
	void FlyingCamAngleDisUpdate(int x, int y);

	//OpenGL Matrix for picking
	GLint viewport[4];
	GLdouble MV[16];
	GLdouble P[16];
	float oldX = 0, oldY = 0;		 //Mouse movement
	float rX = 30, rY = 90;			 //Mouse rotation
	int state = 1;					 //Mouse Click sign
	float dist = -25;				 //Initial FlyingCam displacement

	//FlyingCam
	Vector3d Up = Vector3d(0, 1, 0);		//for picking
	Vector3d Right;
	Vector3d viewDir;
private:

};

FlyingCam::FlyingCam()
{
}

FlyingCam::~FlyingCam()
{
}
void FlyingCam::FlyingCamCursorUpdate(int mousebutton, int mousestate, int mouseX, int mouseY)
{
	if (mousestate == GLUT_DOWN && mousebutton == GLUT_LEFT_BUTTON)
	{
		//if (button == GLUT_LEFT_BUTTON)
		//{
		//	//cout << "ms is called" << endl;
		//	cam.oldX = x;
		//	cam.oldY = y;
		//}
		
		oldX = mouseX;
		oldY = mouseY;
	}

	if (mousebutton == GLUT_MIDDLE_BUTTON)
	{
		oldX = mouseX;
		oldY = mouseY;
		state = 0;
	}
	else		state = 1;
}
void FlyingCam::FlyingCamAngleDisUpdate(int x, int y)
{
	if (state == 0)
		dist *= (1 + (y - oldY) / 120.0f);
	else
	{
		rY += (x - oldX) / 5.0f;
		rX += (y - oldY) / 5.0f;

	}

	oldX = x;
	oldY = y;
}