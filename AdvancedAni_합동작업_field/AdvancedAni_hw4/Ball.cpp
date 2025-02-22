#include "Ball.h"

Ball::Ball() {
	radius = 0.3;
	mass = 0.1;
	friction = 0.1;
	startPos = Vector3f(-0.7, radius, 1);
	bounce = 0.8;

	position = startPos + Vector3f(0, 0, 0);
	prevPos = position;
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
	prevPos = position;
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
	prevPos = position;
	forces = Vector3f(0, 0, 0);
	velocity = Vector3f(0, 0, 0);
	oldVelocity = Vector3f(0, 0, 0);
	newVelocity = Vector3f(0, 0, 0);
}

void Ball::setVelocity(Vector3f vel) {
	velocity = vel;
	oldVelocity = Vector3f(0, 0, 0);
	newVelocity = Vector3f(0, 0, 0);
}

void Ball::updatePosition(float deltaTime) {
	Vector3f newPos = position + (1-damping/100)*velocity*deltaTime + 0.5*(gravity*1000 + forces/mass)*deltaTime*deltaTime;
	velocity = (newPos - position) / deltaTime;
	//velocity = velocity + (gravity * 1000 + forces / mass)*deltaTime;
	prevPos = position;
	position = newPos;
	//forces = Vector3f(0, 0, 0);
}

void Ball::updateVelocity(Vector3f forces) {
	float factor = 0.7;
	if (velocity.norm() > 8) factor *= velocity[2] * 0.8;
	forces[2] *= factor;
	Vector3f newVel = velocity + -forces / mass * 0.8;//0.7
	velocity = newVel;
}


void Ball::collideWithGround() {
	if (position[1] < radius) {
		position[1] = radius;
		velocity[1] = -bounce * velocity[1];
		velocity[0] = (1 - friction) * velocity[0];
		velocity[2] = (1 - friction) * velocity[2];
	}
}

void Ball::collideWithBalls(vector<Ball> balls) {
	newVelocity = velocity;
	for (Ball &other : balls) {
		float distance = (position - other.getPosition()).norm();
		if (distance != 0 && distance <= radius * 2) {
			newVelocity -= (velocity - other.getVelocity()).dot(position - other.getPosition()) / (distance*distance) * (position - other.getPosition());
		}
	}
}

void Ball::applyChanges() {
	velocity = newVelocity;
}