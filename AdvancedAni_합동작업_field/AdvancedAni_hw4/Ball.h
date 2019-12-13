#pragma once

#include <vector>
#include <Eigen/Dense>
#include <GL/glut.h>

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

	Ball();
	Ball(Vector3f startPos, float radius, float mass, float friction, float bounce);
	void render();
	void updatePosition(float deltaTime);
	void reset();
	void setPosition(Vector3f pos);
	Vector3f getPosition() { return position; }

private:
	Vector3f velocity;
	Vector3f oldVelocity;
	Vector3f newVelocity;
};