#include "Ball.h"

Ball::Ball() {
	radius = 0.4;
	mass = 1;
	friction = 0.1;
	startPos = Vector3f(-0.7, radius, 1);

	position = startPos + Vector3f(0, 0, 0);
	forces = Vector3f(0, 0, 0);
	velocity = Vector3f(0, 0, 0);
	oldVelocity = Vector3f(0, 0, 0);
	newVelocity = Vector3f(0, 0, 0);
}

Ball::Ball(Vector3f p, float r, float m, float f, float b) {
	radius = r;
	mass = m;
	friction = f;
	startPos = p;
	bounce = b;

	position = startPos + Vector3f(0, 0, 0);
	forces = Vector3f(0, 0, 0);
	velocity = Vector3f(0, 0, 0);
	oldVelocity = Vector3f(0, 0, 0);
	newVelocity = Vector3f(0, 0, 0);
}

void Ball::render() {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(0.5, 0.5, 0.5);
	glTranslatef(position(0), position(1), position(2));
	glutSolidSphere(radius, 20, 20);

	glPopMatrix();
	glDisable(GL_LIGHTING);
}

void Ball::reset() {
	position = startPos + Vector3f(0, 0, 0);
	forces = Vector3f(0, 0, 0);
	velocity = Vector3f(0, 0, 0);
	oldVelocity = Vector3f(0, 0, 0);
	newVelocity = Vector3f(0, 0, 0);
}

void Ball::setPosition(Vector3f pos) {
	position = pos + Vector3f(0, 0, 0);
	forces = Vector3f(0, 0, 0);
	velocity = Vector3f(0, 0, 0);
	oldVelocity = Vector3f(0, 0, 0);
	newVelocity = Vector3f(0, 0, 0);
}

void Ball::updatePosition(float deltaTime) {
	return;
}