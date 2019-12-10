#pragma once

#include <iostream>
#include <vector>
#include <Eigen/Dense>
#include <GL/glut.h>

using namespace std;
using namespace Eigen;

struct DistanceConstraint { int p1, p2;   float rest_length, k; float k_prime; };
struct BendingConstraint { int p1, p2, p3;   float rest_length, w, k; float k_prime; };

class PBD_Cloth
{
public:
	PBD_Cloth();
	~PBD_Cloth();
	void OnShutdown();

	void initialization();
	int getIndex(int i, int j) {return j * (numX + 1) + i;	}

	void StepPhysics(float dt);
	
	float GetArea(int a, int b, int c);
	Vector3f GetNormal(int ind0, int ind1, int ind2);
	
	void AddDistanceConstraint(int a, int b, float k);
	void AddBendingConstraint(int pa, int pb, int pc, float k);
	void DrawCloth();

	int numX = 20, numY = 20; //these ar the number of quads
	size_t total_points = (numX + 1)*(numY + 1);
	int sizeCJ = 4;
	float hsize = sizeCJ / 2.0f;


	vector<Vector3f>		 pos;			//predicted position
	vector<Vector3f>		 tmp_pos;
	vector<Vector3f>		 vel;
	vector<Vector3f>		 force;
	vector<float>			 W;				//inverse particle mass 
	vector<Vector3f>		 Ri;			//Ri = Xi-Xcm 

	//need post WORKING
	int selected_index = -1;
	float global_dampening = 0.98f; //DevO: 24.07.2011

	vector<GLushort> indices;
	vector<DistanceConstraint> d_constraints;

	vector<BendingConstraint> b_constraints;
	vector<float> phi0; //initial dihedral angle between adjacent triangles

	size_t solver_iterations = 2; //number of solver iterations per step. PBD  

	float kBend = 0.5f;
	float kStretch = 0.25f;
	float kDamp = 0.00125f;
	Vector3f gravity = Vector3f(0.0f, -0.00981f, 0.0f);

	float mass = 1.f / (total_points);





private:

};

