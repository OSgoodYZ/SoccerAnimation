#pragma once

#include <vector>
#include <Eigen/Dense>
#include <GL/glut.h>
#include <iostream>

using namespace std;
using namespace Eigen;

#define PI 3.1415926536f


class Ball {
public:
	Vector3f position;
	Vector3f startPos;
	float radius;
	float mass;
	float friction;
	float bounce;
	Vector3f forces;

	Vector3f gravity = Vector3f(0.0f, -0.00981f, 0.0f);
	float damping = 0.05;

	Ball();
	Ball(Vector3f startPos, float radius, float mass, float friction, float bounce);
	void render();
	void updatePosition(float deltaTime);
	void updateVelocity(Vector3f forces);
	void collideWithGround();
	void reset();
	void setPosition(Vector3f pos);
	void setVelocity(Vector3f vel);
	Vector3f getPosition() { return position; }
	Vector3f getPreviousPosition() {return prevPos; }

private:
	Vector3f velocity;
	Vector3f oldVelocity;
	Vector3f newVelocity;
	Vector3f prevPos;
};