#include "PBD_Cloth.h"

PBD_Cloth::PBD_Cloth()
{
	numX = 20;
	numY = 20; 
	total_points = (numX + 1)*(numY + 1);
	sizeCJ = 4;
	hsize = sizeCJ / 2.0f;

	solver_iterations = 2;

	kBend = 0.5f;
	kStretch = 0.25f;
	kDamp = 0.00125f;
	gravity = Vector3f(0.0f, -0.00981f, 0.0f);

	selected_index = -1;
	global_dampening = 0.98f;
}

PBD_Cloth::~PBD_Cloth()
{
	OnShutdown();
}
void PBD_Cloth::initialization()
{

	size_t i = 0, j = 0, count = 0;
	int l1 = 0, l2 = 0;
	float ypos = 7.0f;
	int v = numY + 1;
	int u = numX + 1;

	indices.resize(numX*numY * 2 * 3);

	pos.resize(total_points);
	tmp_pos.resize(total_points);
	vel.resize(total_points);
	force.resize(total_points);
	Ri.resize(total_points);

	//fill in positions
	for (int j = 0; j <= numY; j++) {
		for (int i = 0; i <= numX; i++) {
			pos[count++] = Vector3f(((float(i) / (u - 1)) * 2 - 1)* hsize, sizeCJ + 1, ((float(j) / (v - 1))* sizeCJ));
		}
	}
	for (i = 0; i < total_points; i++)
	{

		vel[i] = Vector3f(0, 0, 0);
	}
	
	///DevO: 24.07.2011
	W.resize(total_points);
	for (i = 0; i < total_points; i++) {
		W[i] = 1.0f / mass;
	}
	/// 2 Fixed Points 
	W[0] = 0.0;
	W[numX] = 0.0;

	//memcpy(&tmp_pos[0].x, &pos[0].x, sizeof(Vector3f)*total_points);

	//fill in velocities    
	//memset(&(vel[0].x), 0, total_points * sizeof(Vector3f));

	//fill in indices
	GLushort* id = &indices[0];
	for (int i = 0; i < numY; i++) {
		for (int j = 0; j < numX; j++) {
			int i0 = i * (numX + 1) + j;
			int i1 = i0 + 1;
			int i2 = i0 + (numX + 1);
			int i3 = i2 + 1;
			if ((j + i) % 2) {
				*id++ = i0; *id++ = i2; *id++ = i1;
				*id++ = i1; *id++ = i2; *id++ = i3;
			}
			else {
				*id++ = i0; *id++ = i2; *id++ = i3;
				*id++ = i0; *id++ = i3; *id++ = i1;
			}
		}
	}

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glPolygonMode(GL_BACK, GL_LINE);
	glPointSize(5);

	//wglSwapIntervalEXT(0); //for 수직 동기화

	//check the damping values
	if (kStretch > 1)
		kStretch = 1;
	if (kStretch < 0)
		kStretch = 0;
	if (kBend > 1)
		kBend = 1;
	if (kBend < 0)
		kBend = 0;
	if (kDamp > 1)
		kDamp = 1;
	if (kDamp < 0)
		kDamp = 0;
	if (global_dampening > 1)
		global_dampening = 1;

	//setup constraints
	// Horizontal
	for (l1 = 0; l1 < v; l1++)   // v
		for (l2 = 0; l2 < (u - 1); l2++) {
			AddDistanceConstraint((l1 * u) + l2, (l1 * u) + l2 + 1, kStretch);
		}

	// Vertical
	for (l1 = 0; l1 < (u); l1++)
		for (l2 = 0; l2 < (v - 1); l2++) {
			AddDistanceConstraint((l2 * u) + l1, ((l2 + 1) * u) + l1, kStretch);
		}


	// Shearing distance constraint
	for (l1 = 0; l1 < (v - 1); l1++)
		for (l2 = 0; l2 < (u - 1); l2++) {
			AddDistanceConstraint((l1 * u) + l2, ((l1 + 1) * u) + l2 + 1, kStretch);
			AddDistanceConstraint(((l1 + 1) * u) + l2, (l1 * u) + l2 + 1, kStretch);
		}


	// create bending constraints   

//add vertical constraints
	for (int i = 0; i <= numX; i++) {
		for (int j = 0; j < numY - 1; j++) {
			AddBendingConstraint(getIndex(i, j), getIndex(i, (j + 1)), getIndex(i, j + 2), kBend);
		}
	}
	//add horizontal constraints
	for (int i = 0; i < numX - 1; i++) {
		for (int j = 0; j <= numY; j++) {
			AddBendingConstraint(getIndex(i, j), getIndex(i + 1, j), getIndex(i + 2, j), kBend);
		}
	}

	////############################			need working
	////create a basic ellipsoid object
	//ellipsoid = glm::translate(glm::mat4(1), glm::vec3(0, 2, 0));
	//ellipsoid = glm::rotate(ellipsoid, 45.0f, glm::vec3(1, 0, 0));
	//ellipsoid = glm::scale(ellipsoid, glm::vec3(fRadius, fRadius, fRadius / 2));
	//inverse_ellipsoid = glm::inverse(ellipsoid);


}

void PBD_Cloth::StepPhysics(float dt)
{
	ComputeForces();
	IntegrateExplicitWithDamping(dt);

	// for collision constraints
	UpdateInternalConstraints(dt);
	UpdateExternalConstraints();

	Integrate(dt);

}
float PBD_Cloth::GetArea(int a, int b, int c)
{
	Vector3f edge1 = pos[b] - pos[a];
	Vector3f edge2 = pos[c] - pos[a];
	return 0.5f *(edge1.cross(edge2)).norm();

}
Vector3f PBD_Cloth::GetNormal(int ind0, int ind1, int ind2)
{
	Vector3f edge1 = pos[ind0] - pos[ind1];
	Vector3f edge2 = pos[ind2] - pos[ind1];
	Vector3f tempnormal = edge1.cross(edge2);
	tempnormal.normalize();
	return tempnormal;

}
void PBD_Cloth::AddDistanceConstraint(int a, int b, float k)
{
	DistanceConstraint c;
	c.p1 = a;
	c.p2 = b;
	c.k = k;
	c.k_prime = 1.0f - pow((1.0f - c.k), 1.0f / solver_iterations);  //1.0f-pow((1.0f-c.k), 1.0f/ns);

	if (c.k_prime > 1.0)
		c.k_prime = 1.0;

	Vector3f deltaP = pos[c.p1] - pos[c.p2];
	c.rest_length = deltaP.norm();

	d_constraints.push_back(c);
}
void PBD_Cloth::AddBendingConstraint(int pa, int pb, int pc, float k) {
	BendingConstraint c;
	c.p1 = pa;
	c.p2 = pb;
	c.p3 = pc;

	c.w = W[pa] + W[pb] + 2 * W[pc];
	
	Vector3f center = 0.3333f * (pos[pa] + pos[pb] + pos[pc]);
	c.rest_length = (pos[pc] - center).norm();
	c.k = k;
	c.k_prime = 1.0f - pow((1.0f - c.k), 1.0f / solver_iterations);  //1.0f-pow((1.0f-c.k), 1.0f/ns);
	if (c.k_prime > 1.0)
		c.k_prime = 1.0;
	b_constraints.push_back(c);

}
void PBD_Cloth::DrawCloth()
{
	glDisable(GL_LIGHTING);
	//draw polygons
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(1, 1, 1);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < indices.size(); i += 3) {
		Vector3f p1 = pos[indices[i]];
		Vector3f p2 = pos[indices[i + 1]];
		Vector3f p3 = pos[indices[i + 2]];
		glVertex3f(p1[0], p1[1], p1[2]);
		glVertex3f(p2[0], p2[1], p2[2]);
		glVertex3f(p3[0], p3[1], p3[2]);
	}
	glEnd();

	glColor3f(1, 0, 0);
	//draw points
	glBegin(GL_POINTS);
	for (int i = 0; i < total_points; i++) {
		Vector3f p = pos[i];
		int is = (i == selected_index);
		glColor3f((float)!is, (float)is, (float)is);
		glVertex3f(p[0], p[1], p[2]);
	}
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);
}

void PBD_Cloth::OnShutdown()
{
	d_constraints.clear();
	b_constraints.clear();
	indices.clear();
	pos.clear();
	force.clear();
	vel.clear();
	phi0.clear();
	W.clear();
	tmp_pos.clear();
	Ri.clear();
}
void PBD_Cloth::ComputeForces() {
	
	for (int i = 0; i < total_points; i++) {
		force[i] = Vector3f(0, 0, 0);

		//add gravity force
		if (W[i] > 0)
			force[i] += gravity;
	}
}
void PBD_Cloth::IntegrateExplicitWithDamping(float deltaTime)
{
	float deltaTimeMass = deltaTime;
	size_t i = 0;
	
	Vector3f Xcm = Vector3f(0,0,0);
	Vector3f Vcm = Vector3f(0, 0, 0);
	float sumM = 0;
	for (i = 0; i < total_points; i++) {

		
		vel[i] *= global_dampening; //global velocity dampening !!!      
		vel[i] = vel[i] + (force[i] * deltaTime)*W[i];

		//calculate the center of mass's position 
		//and velocity for damping calc
		Xcm += (pos[i] * mass);
		Vcm += (vel[i] * mass);
		sumM += mass;
	}
	Xcm /= sumM;
	Vcm /= sumM;
	
	Matrix3f I;
	I << 1,0,0,
		 0,1,0,
		 0,0,1;
	cout << I << endl;
	
	Vector3f L = Vector3f(0, 0, 0);
	Vector3f w = Vector3f(0, 0, 0);//angular velocity


	for (i = 0; i < total_points; i++) {
		Ri[i] = (pos[i] - Xcm);

		L += Ri[i].cross(mass*vel[i]);

		Matrix3f tmp;
		tmp << 0, -Ri[i][2], Ri[i][1],
			   Ri[i][2], 0, -Ri[i][0],
			   -Ri[i][1], Ri[i][0], 0;

		I += tmp * tmp.transpose() *mass;
		//I += (tmp*glm::transpose(tmp))*mass;
	}

	w = I.inverse() *L;
	

	//apply center of mass damping
	for (i = 0; i < total_points; i++) {
		Vector3f delVi = Vcm + w.cross(Ri[i]) - vel[i];
		vel[i] += kDamp * delVi;
	}

	//calculate predicted position
	for (i = 0; i < total_points; i++) {
		if (W[i] <= 0.0) {
			tmp_pos[i] = pos[i]; //fixed points
		}
		else {
			tmp_pos[i] = pos[i] + (vel[i] * deltaTime);
		}
	}
}

void PBD_Cloth::Integrate(float deltaTime)
{
	float inv_dt = 1.0f / deltaTime;
	
	for (int i = 0; i < total_points; i++)
	{
		vel[i] = (tmp_pos[i] - pos[i])*inv_dt;
		pos[i] = tmp_pos[i];
	}
}

void PBD_Cloth::UpdateDistanceConstraint(int i)
{

	DistanceConstraint c = d_constraints[i];
	Vector3f dir = tmp_pos[c.p1] - tmp_pos[c.p2];

	float len = dir.norm();
	if (len <= EPSILON)
		return;

	float w1 = W[c.p1];
	float w2 = W[c.p2];
	float invMass = w1 + w2;
	if (invMass <= EPSILON)
		return;

	Vector3f dP = (1.0f / invMass) * (len - c.rest_length) * (dir / len)* c.k_prime;
	if (w1 > 0.0)
		tmp_pos[c.p1] -= dP * w1;

	if (w2 > 0.0)
		tmp_pos[c.p2] += dP * w2;
}

void PBD_Cloth::UpdateBendingConstraint(int index)
{
	size_t i = 0;
	BendingConstraint c = b_constraints[index];

	float global_k = global_dampening * 0.01f;
	Vector3f center = 0.3333f * (tmp_pos[c.p1] + tmp_pos[c.p2] + tmp_pos[c.p3]);
	Vector3f dir_center = tmp_pos[c.p3] - center;

	float dist_center = dir_center.norm();
	//float dist_center = glm::length(dir_center);

	float diff = 1.0f - ((global_k + c.rest_length) / dist_center);
	Vector3f dir_force = dir_center * diff;
	Vector3f fa = c.k_prime * ((2.0f*W[c.p1]) / c.w) * dir_force;
	Vector3f fb = c.k_prime * ((2.0f*W[c.p2]) / c.w) * dir_force;
	Vector3f fc = -c.k_prime * ((4.0f*W[c.p3]) / c.w) * dir_force;

	if (W[c.p1] > 0.0) {
		tmp_pos[c.p1] += fa;
	}
	if (W[c.p2] > 0.0) {
		tmp_pos[c.p2] += fb;
	}
	if (W[c.p3] > 0.0) {
		tmp_pos[c.p3] += fc;
	}
}
void PBD_Cloth::GroundCollision() //DevO: 24.07.2011
{
	for (int i = 0; i < total_points; i++) {
		if (tmp_pos[i][1] < 0) //collision with ground  axis y
			tmp_pos[i][1] = 0;
	}
}

//void PBD_Cloth::EllipsoidCollision() 
//{
//	for (size_t i = 0; i < total_points; i++) {
//		glm::vec4 X_0 = (inverse_ellipsoid*glm::vec4(tmp_X[i], 1));
//		glm::vec3 delta0 = glm::vec3(X_0.x, X_0.y, X_0.z) - center;
//		float distance = glm::length(delta0);
//		if (distance < 1.0f) {
//			delta0 = (radius - distance) * delta0 / distance;
//
//			// Transform the delta back to original space
//			glm::vec3 delta;
//			glm::vec3 transformInv;
//			transformInv = glm::vec3(ellipsoid[0].x, ellipsoid[1].x, ellipsoid[2].x);
//			transformInv /= glm::dot(transformInv, transformInv);
//			delta.x = glm::dot(delta0, transformInv);
//			transformInv = glm::vec3(ellipsoid[0].y, ellipsoid[1].y, ellipsoid[2].y);
//			transformInv /= glm::dot(transformInv, transformInv);
//			delta.y = glm::dot(delta0, transformInv);
//			transformInv = glm::vec3(ellipsoid[0].z, ellipsoid[1].z, ellipsoid[2].z);
//			transformInv /= glm::dot(transformInv, transformInv);
//			delta.z = glm::dot(delta0, transformInv);
//			tmp_X[i] += delta;
//			V[i] = glm::vec3(0);
//		}
//	}
//}
void PBD_Cloth::UpdateExternalConstraints()
{
	int temp = 0;
	//EllipsoidCollision();
}
void PBD_Cloth::UpdateInternalConstraints(float deltaTime)
{
	size_t i = 0;

	//printf(" UpdateInternalConstraints \n ");
	for (size_t si = 0; si < solver_iterations; ++si) {
		for (i = 0; i < d_constraints.size(); i++) {
			UpdateDistanceConstraint(i);
		}
		for (i = 0; i < b_constraints.size(); i++) {
			UpdateBendingConstraint(i);
		}
		GroundCollision();
	}
}