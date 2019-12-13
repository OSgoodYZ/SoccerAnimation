#include "BVHObject.h"
#include <vector>
#include <Eigen/Dense>

int motion_kicker = -1;
int motion_keeper = -1;

using namespace std;

namespace {
	GLUquadricObj* quadObj = 0;
}

void BVHObject::clear() {
	ready = false;
	nFrames = 0;
	interval = 0.0;

	motion.clear();
	joints.clear();
	channels.clear();
}

void BVHObject::init(string fileName) {

	if (quadObj == 0) {
		quadObj = gluNewQuadric();
	}

#define BUFFER_SIZE (1024 * 4)

	ifstream  file;
	char      line[BUFFER_SIZE];
	char* token;
	char      separater[] = " :,\t\r";  // in linux : add \r
	Index     joint = -1;
	Index     newJoint = -1;
	bool      isSite = false;

	vector<Index> stack;

	clear();

	name = fileName;

	file.open(fileName, ios::in);
	if (!file.is_open()) {
		return;
	}

	while (!file.eof()) {

		if (file.eof()) {
			file.close();
			return;
		}

		file.getline(line, BUFFER_SIZE);

		token = strtok(line, separater);

		// empty line
		if (token == NULL) { continue; }

		// joint block starts
		if (strcmp(token, "{") == 0) {
			// push current joint into stack
			stack.push_back(joint);
			joint = newJoint;
			continue;
		}
		// joint block ends
		if (strcmp(token, "}") == 0) {
			// pop joint from stack
			joint = stack.back();
			stack.pop_back();
			isSite = false;
			continue;
		}

		// joint information
		if ((strcmp(token, "ROOT") == 0) ||
			(strcmp(token, "JOINT") == 0)) {

			// joint data creation
			kicker::Joint tmpJoint = kicker::Joint();
			newJoint = static_cast<int>(joints.size());
			tmpJoint.index = newJoint;
			tmpJoint.parent = joint;
			tmpJoint.hasSite = false;
			tmpJoint.offset = JH::Vector3d(0.0, 0.0, 0.0);
			tmpJoint.site = JH::Vector3d(0.0, 0.0, 0.0);
			if (joint >= 0) {
				joints[joint].children.push_back(newJoint);
			}

			// name of joint
			token = strtok(NULL, "");
			while (*token == ' ') {
				token++;
			}
			tmpJoint.name = string(token);

			// register
			jointNameToIndex[tmpJoint.name] = newJoint;
			joints.push_back(tmpJoint);

			continue;
		}

		// start the end point
		if ((strcmp(token, "End") == 0)) {
			newJoint = joint;
			isSite = true;
			continue;
		}

		// offset or end point
		if (strcmp(token, "OFFSET") == 0) {

			JH::Vector3d offset;

			// get x, y, and z
			token = strtok(NULL, separater);
			offset.x = token ? atof(token) : 0.0;
			token = strtok(NULL, separater);
			offset.y = token ? atof(token) : 0.0;
			token = strtok(NULL, separater);
			offset.z = token ? atof(token) : 0.0;

			if (isSite) {
				// end point
				joints[joint].hasSite = true;
				joints[joint].site = offset;
			}
			else {
				// offset
				joints[joint].offset = offset;
			}
			continue;
		}

		// channels
		if (strcmp(token, "CHANNELS") == 0) {
			// number of channles
			token = strtok(NULL, separater);
			joints[joint].channels.resize(token ? atoi(token) : 0);

			// read
			for (int i = 0; i < joints[joint].channels.size(); ++i) {
				// make
				kicker::Channel channel = kicker::Channel();
				channel.joint = joints[joint].index;
				channel.index = static_cast<int>(channels.size());
				joints[joint].channels[i] = channel.index;

				// check the type of channel
				token = strtok(NULL, separater);
				if (strcmp(token, "Xposition") == 0)
					channel.type = kicker::Channel::X_POSITION;
				else if (strcmp(token, "Yposition") == 0)
					channel.type = kicker::Channel::Y_POSITION;
				else if (strcmp(token, "Zposition") == 0)
					channel.type = kicker::Channel::Z_POSITION;
				else if (strcmp(token, "Zrotation") == 0)
					channel.type = kicker::Channel::Z_ROTATION;
				else if (strcmp(token, "Xrotation") == 0)
					channel.type = kicker::Channel::X_ROTATION;
				else if (strcmp(token, "Yrotation") == 0)
					channel.type = kicker::Channel::Y_ROTATION;

				channels.push_back(channel);
			}
		}

		if (strcmp(token, "HIERARCHY") == 0) {
			continue;
		}

		// move to motion section
		if (strcmp(token, "MOTION") == 0)
			break;
	}

	// motion data
	file.getline(line, BUFFER_SIZE);

	token = strtok(line, separater);
	if (strcmp(token, "Frames") != 0) {
		file.close();
		return;
	}
	token = strtok(NULL, separater);
	if (token == NULL) {
		file.close();
		return;
	}
	nFrames = atoi(token);

	file.getline(line, BUFFER_SIZE);
	token = strtok(line, ":");
	if (strcmp(token, "Frame Time") != 0) {
		file.close();
		return;
	}
	token = strtok(NULL, separater);
	if (token == NULL) {
		file.close();
		return;
	}
	interval = atof(token);

	nChannels = static_cast<int>(channels.size());
	motion.resize(nFrames);

	// read
	for (int i = 0; i < nFrames; ++i) {
		file.getline(line, BUFFER_SIZE);
		token = strtok(line, separater);
		for (int j = 0; j < nChannels; ++j) {
			if (token == NULL) {
				file.close();
				return;
			}
			motion[i].push_back(atof(token));
			token = strtok(NULL, separater);
		}
	}

	file.close();

	// succeeded
	ready = true;

	return;
#undef BUFFER_SIZE
}

void BVHObject::render(int frameNum) {
	Pose   pose = motion[frameNum];
	Index  rootIndex = 0;

	renderJoint(rootIndex, pose, frameNum);
}

void BVHObject::pose_save1(int frameNum) {
	save_pose1 = motion[frameNum];
}

void BVHObject::pose_save2(int frameNum) {
	save_pose2 = motion[frameNum];
}

BVHObject::BVHObject() : ready(false), nFrames(0), interval(0.0), renderingJoint(true), renderingBone(true) {

}

BVHObject::~BVHObject() {
	if (quadObj != 0) {
		gluDeleteQuadric(quadObj);
	}
}

//////////////////////////////////////
// private
//////////////////////////////////////

void BVHObject::renderJoint(Index joint, Pose pose, int frameNum) {
	kicker::Joint& j = joints[joint];

	gluQuadricDrawStyle(quadObj, GLU_FILL);
	gluQuadricNormals(quadObj, GLU_SMOOTH);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	float pos_kicker_x = 0, pos_kicker_y = 0, pos_kicker_z = 0;

	//blending 
	int blending_frame = 10;
	float blending_ratio = 1;


	if (frameNum < blending_frame) {
		blending_ratio = -0.5 * cos(3.14 / blending_frame * (frameNum + 1)) + 0.5;  // 0frame -> 0
	}
	else {
		blending_ratio = 1;
	}


	if (j.parent < 0) {
		// if root, translate
		switch (motion_kicker)
		{
		case 2:

			pos_kicker_x = save_pose1[0] - save_pose2[0];
			pos_kicker_y = save_pose1[1] - save_pose2[1];
			pos_kicker_z = save_pose1[2] - save_pose2[2];
			break;
		}
		glTranslated(pose[0] + pos_kicker_x, pose[1] + pos_kicker_y, pose[2] + pos_kicker_z);
	}
	else {
		// if not, translate by offset from the parent
		glTranslated(j.offset[0], j.offset[1], j.offset[2]);
	}

	// rotation from the parent (if root, from the world)
	for (int i = 0; i < j.channels.size(); ++i) {
		kicker::Channel& channel = channels[j.channels[i]];
		if (channel.type == kicker::Channel::X_ROTATION) {
			glRotated(save_pose1[channel.index] * (1 - blending_ratio) + pose[channel.index] * blending_ratio, 1.0, 0.0, 0.0);
		}
		else if (channel.type == kicker::Channel::Y_ROTATION) {
			glRotated(save_pose1[channel.index] * (1 - blending_ratio) + pose[channel.index] * blending_ratio, 0.0, 1.0, 0.0);
		}
		else if (channel.type == kicker::Channel::Z_ROTATION) {
			glRotated(save_pose1[channel.index] * (1 - blending_ratio) + pose[channel.index] * blending_ratio, 0.0, 0.0, 1.0);
		}
	}

	if (renderingBone) {
		glColor3d(1.0, 1.0, 1.0);

		// draw link
		if (j.children.size() == 0) {
			// the origin of joint-coord to the end point
			renderBone(JH::Vector3d(0.0, 0.0, 0.0), j.site);
		}

		for (int i = 0; i < j.children.size(); ++i) {
			// the origin of joint-coord to the next joint
			kicker::Joint& child = joints[j.children[i]];
			renderBone(JH::Vector3d(0.0, 0.0, 0.0), child.offset);
		}
	}

	if (renderingJoint) {
		glColor3d(1.0, 1.0, 1.0);
		
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		gluSphere(quadObj, 3.0, 12, 12);
		//glutSolidSphere(3.0, 12, 12);
		glPopAttrib();
	}

	// recursive
	for (int i = 0; i < j.children.size(); ++i) {
		renderJoint(j.children[i], pose, frameNum);
	}

	glPopMatrix();
}

void BVHObject::renderBone(JH::Vector3d position0, JH::Vector3d position1) {
	GLdouble radius = 2.0;
	GLdouble slices = 8.0;
	GLdouble stack = 3.0;

	JH::Vector3d dir = position1 - position0;
	double   len = dir.length();

	gluQuadricDrawStyle(quadObj, GLU_FILL);
	gluQuadricNormals(quadObj, GLU_SMOOTH);

	dir.normalize();
	JH::Vector3d up(0.0, 1.0, 0.0);
	JH::Vector3d side = up.cross_product(dir);
	if (side.length() < 0.1) {
		up = JH::Vector3d(1.0, 0.0, 0.0);
		side = up.cross_product(dir);
	}
	side.normalize();
	up = dir.cross_product(side);

	GLdouble  m[16] = {
		side.x, side.y, side.z, 0.0,
		up.x,   up.y,   up.z,   0.0,
		dir.x,  dir.y,  dir.z,  0.0,
		0.0,    0.0,    0.0,    1.0
	};

	glPushMatrix();
	glTranslated(position0.x, position0.y, position0.z);
	glMultMatrixd(m);
	gluCylinder(quadObj, radius, radius, len, slices, stack);
	glPopMatrix();
}




void BVHObject2::clear() {
	ready = false;
	nFrames = 0;
	interval = 0.0;

	motion.clear();
	joints.clear();
	channels.clear();
}

void BVHObject2::init(string fileName) {

	if (quadObj == 0) {
		quadObj = gluNewQuadric();
	}

#define BUFFER_SIZE (1024 * 4)

	ifstream  file;
	char      line[BUFFER_SIZE];
	char* token;
	char      separater[] = " :,\t\r";  // in linux : add \r
	Index     joint = -1;
	Index     newJoint = -1;
	bool      isSite = false;

	vector<Index> stack;

	clear();

	name = fileName;

	file.open(fileName, ios::in);
	if (!file.is_open()) {
		return;
	}

	while (!file.eof()) {

		if (file.eof()) {
			file.close();
			return;
		}

		file.getline(line, BUFFER_SIZE);

		token = strtok(line, separater);

		// empty line
		if (token == NULL) { continue; }

		// joint block starts
		if (strcmp(token, "{") == 0) {
			// push current joint into stack
			stack.push_back(joint);
			joint = newJoint;
			continue;
		}
		// joint block ends
		if (strcmp(token, "}") == 0) {
			// pop joint from stack
			joint = stack.back();
			stack.pop_back();
			isSite = false;
			continue;
		}

		// joint information
		if ((strcmp(token, "ROOT") == 0) ||
			(strcmp(token, "JOINT") == 0)) {

			// joint data creation
			keeper::Joint tmpJoint = keeper::Joint();
			newJoint = static_cast<int>(joints.size());
			tmpJoint.index = newJoint;
			tmpJoint.parent = joint;
			tmpJoint.hasSite = false;
			tmpJoint.offset = JH::Vector3d(0.0, 0.0, 0.0);
			tmpJoint.site = JH::Vector3d(0.0, 0.0, 0.0);
			if (joint >= 0) {
				joints[joint].children.push_back(newJoint);
			}

			// name of joint
			token = strtok(NULL, "");
			while (*token == ' ') {
				token++;
			}
			tmpJoint.name = string(token);

			// register
			jointNameToIndex[tmpJoint.name] = newJoint;
			joints.push_back(tmpJoint);

			continue;
		}

		// start the end point
		if ((strcmp(token, "End") == 0)) {
			newJoint = joint;
			isSite = true;
			continue;
		}

		// offset or end point
		if (strcmp(token, "OFFSET") == 0) {

			JH::Vector3d offset;

			// get x, y, and z
			token = strtok(NULL, separater);
			offset.x = token ? atof(token) : 0.0;
			token = strtok(NULL, separater);
			offset.y = token ? atof(token) : 0.0;
			token = strtok(NULL, separater);
			offset.z = token ? atof(token) : 0.0;

			if (isSite) {
				// end point
				joints[joint].hasSite = true;
				joints[joint].site = offset;
			}
			else {
				// offset
				joints[joint].offset = offset;
			}
			continue;
		}

		// channels
		if (strcmp(token, "CHANNELS") == 0) {
			// number of channles
			token = strtok(NULL, separater);
			joints[joint].channels.resize(token ? atoi(token) : 0);

			// read
			for (int i = 0; i < joints[joint].channels.size(); ++i) {
				// make
				keeper::Channel channel = keeper::Channel();
				channel.joint = joints[joint].index;
				channel.index = static_cast<int>(channels.size());
				joints[joint].channels[i] = channel.index;

				// check the type of channel
				token = strtok(NULL, separater);
				if (strcmp(token, "Xposition") == 0)
					channel.type = keeper::Channel::X_POSITION;
				else if (strcmp(token, "Yposition") == 0)
					channel.type = keeper::Channel::Y_POSITION;
				else if (strcmp(token, "Zposition") == 0)
					channel.type = keeper::Channel::Z_POSITION;
				else if (strcmp(token, "Zrotation") == 0)
					channel.type = keeper::Channel::Z_ROTATION;
				else if (strcmp(token, "Xrotation") == 0)
					channel.type = keeper::Channel::X_ROTATION;
				else if (strcmp(token, "Yrotation") == 0)
					channel.type = keeper::Channel::Y_ROTATION;

				channels.push_back(channel);
			}
		}

		if (strcmp(token, "HIERARCHY") == 0) {
			continue;
		}

		// move to motion section
		if (strcmp(token, "MOTION") == 0)
			break;
	}

	// motion data
	file.getline(line, BUFFER_SIZE);

	token = strtok(line, separater);
	if (strcmp(token, "Frames") != 0) {
		file.close();
		return;
	}
	token = strtok(NULL, separater);
	if (token == NULL) {
		file.close();
		return;
	}
	nFrames = atoi(token);

	file.getline(line, BUFFER_SIZE);
	token = strtok(line, ":");
	if (strcmp(token, "Frame Time") != 0) {
		file.close();
		return;
	}
	token = strtok(NULL, separater);
	if (token == NULL) {
		file.close();
		return;
	}
	interval = atof(token);

	nChannels = static_cast<int>(channels.size());
	motion.resize(nFrames);

	// read
	for (int i = 0; i < nFrames; ++i) {
		file.getline(line, BUFFER_SIZE);
		token = strtok(line, separater);
		for (int j = 0; j < nChannels; ++j) {
			if (token == NULL) {
				file.close();
				return;
			}
			motion[i].push_back(atof(token));
			token = strtok(NULL, separater);
		}
	}

	file.close();

	// succeeded
	ready = true;

	return;
#undef BUFFER_SIZE
}

void BVHObject2::render(int frameNum) {
	Pose   pose = motion[frameNum];
	Index  rootIndex = 0;

	renderJoint(rootIndex, pose, frameNum);
}

void BVHObject2::pose_save1(int frameNum) {
	save_pose1 = motion[frameNum];
}

void BVHObject2::pose_save2(int frameNum) {
	save_pose2 = motion[frameNum];
}

BVHObject2::BVHObject2() : ready(false), nFrames(0), interval(0.0), renderingJoint(true), renderingBone(true) {

}

BVHObject2::~BVHObject2() {
	if (quadObj != 0) {
		gluDeleteQuadric(quadObj);
	}
}

//////////////////////////////////////
// private
//////////////////////////////////////

void BVHObject2::renderJoint(Index joint, Pose pose, int frameNum) {
	keeper::Joint& j = joints[joint];

	gluQuadricDrawStyle(quadObj, GLU_FILL);
	gluQuadricNormals(quadObj, GLU_SMOOTH);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	int blending_frame = 10; //blending
	float blending_ratio = 1;

	if (frameNum < blending_frame) {
		blending_ratio = -0.5 * cos(3.14 / blending_frame * (frameNum + 1)) + 0.5;  // 0frame -> 0
	}
	else {
		blending_ratio = 1;
	}

	float pos_keeper_x = 0, pos_keeper_y = 0, pos_keeper_z = 0;

	if (j.parent < 0) {
		// if root, translate
		switch (motion_keeper)
		{
		case -1:  //-4.8139 36.7156 -5.6347 ~ -10.0849 36.7335 -14.3680
			pos_keeper_z = 310;
			break;
		case 1: // 38.068699 36.393200 -5.382200  ~ 39.483398 36.247002 -3.139300
			pos_keeper_x = -save_pose1[0] + save_pose2[0];
			pos_keeper_y = -save_pose1[1] + save_pose2[1];
			pos_keeper_z = -save_pose1[2] + save_pose2[2] + 310;
			break;
		case 2: // -4.562700 36.605099 -3.237100
			pos_keeper_x = -save_pose1[0] + save_pose2[0] + 38.068699 + 10.0849;
			pos_keeper_y = -save_pose1[1] + save_pose2[1];
			pos_keeper_z = -save_pose1[2] + save_pose2[2] + 310;// +14.3680 - 5.382200;
			break;
		}
		glTranslated(-pose[0] + pos_keeper_x, pose[1] + pos_keeper_y, -pose[2] + pos_keeper_z); // root z rotation 180 degree
	}
	else {
		// if not, translate by offset from the parent
		glTranslated(j.offset[0], j.offset[1], j.offset[2]);
	}

	// rotation from the parent (if root, from the world)
	for (int i = 0; i < j.channels.size(); ++i) {
		keeper::Channel& channel = channels[j.channels[i]];
		if (channel.type == keeper::Channel::X_ROTATION) {
			if (channel.index == 3)
				glRotated(-180 - pose[channel.index], 1.0, 0.0, 0.0); // root z rotation 180 degree
			else {
				glRotated(save_pose1[channel.index] * (1 - blending_ratio) + pose[channel.index] * blending_ratio, 1.0, 0.0, 0.0);
			}
		}
		else if (channel.type == keeper::Channel::Y_ROTATION) {
			if (channel.index == 4)
				glRotated(-pose[channel.index], 0.0, 1.0, 0.0); // root z rotation 180 degree
			else {
				glRotated(save_pose1[channel.index] * (1 - blending_ratio) + pose[channel.index] * blending_ratio, 0.0, 1.0, 0.0);
			}
		}
		else if (channel.type == keeper::Channel::Z_ROTATION) {
			if (channel.index == 5)
				glRotated(pose[channel.index] + 180, 0.0, 0.0, 1.0); // root z rotation 180 degree
			else {
				glRotated(save_pose1[channel.index] * (1 - blending_ratio) + pose[channel.index] * blending_ratio, 0.0, 0.0, 1.0);
			}
		}
	}

	if (renderingBone) {
		glColor3d(1.0, 1.0, 1.0);

		// draw link
		if (j.children.size() == 0) {
			// the origin of joint-coord to the end point
			renderBone(JH::Vector3d(0.0, 0.0, 0.0), j.site);
		}

		for (int i = 0; i < j.children.size(); ++i) {
			// the origin of joint-coord to the next joint
			keeper::Joint& child = joints[j.children[i]];
			renderBone(JH::Vector3d(0.0, 0.0, 0.0), child.offset);
		}
	}

	if (renderingJoint) {
		glColor3d(1.0, 1.0, 1.0);
		//gluSphere(quadObj, 1.0, 12, 12);
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		gluSphere(quadObj, 1.0, 12, 12);
		//glutSolidSphere(3.0, 12, 12);
		glPopAttrib();
	}

	// recursive
	for (int i = 0; i < j.children.size(); ++i) {
		renderJoint(j.children[i], pose, frameNum);
	}

	glPopMatrix();
}

void BVHObject2::renderBone(JH::Vector3d position0, JH::Vector3d position1) {
	GLdouble radius = 0.7;
	GLdouble slices = 8.0;
	GLdouble stack = 3.0;

	JH::Vector3d dir = position1 - position0;
	double   len = dir.length();

	gluQuadricDrawStyle(quadObj, GLU_FILL);
	gluQuadricNormals(quadObj, GLU_SMOOTH);

	dir.normalize();
	JH::Vector3d up(0.0, 1.0, 0.0);
	JH::Vector3d side = up.cross_product(dir);
	if (side.length() < 0.1) {
		up = JH::Vector3d(1.0, 0.0, 0.0);
		side = up.cross_product(dir);
	}
	side.normalize();
	up = dir.cross_product(side);

	GLdouble  m[16] = {
		side.x, side.y, side.z, 0.0,
		up.x,   up.y,   up.z,   0.0,
		dir.x,  dir.y,  dir.z,  0.0,
		0.0,    0.0,    0.0,    1.0
	};

	glPushMatrix();
	glTranslated(position0.x, position0.y, position0.z);
	glMultMatrixd(m);
	gluCylinder(quadObj, radius, radius, len, slices, stack);
	glPopMatrix();
}

