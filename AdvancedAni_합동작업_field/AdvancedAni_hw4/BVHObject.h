#pragma once

#include <map>

#include <stdio.h>
#include <string.h>
#include <fstream>
//#include <GL/gl.h>
//#include <GL/glu.h>
#include <GL/glut.h>
#include "Joint.h"
#include "Channel.h"


class BVHObject {
	typedef int                 Index;
	typedef std::vector<double> Pose;

public:
	std::vector<kicker::Joint>   joints;
	std::vector<kicker::Channel> channels;
	std::vector<Pose>    motion;

	std::map<std::string, Index> jointNameToIndex;

	double       interval;
	int          nFrames;     // equal to motion.size()
	int          nChannels;   // equal to channels.size();
	bool         ready;
	std::string  name;

	bool         renderingJoint;
	bool         renderingBone;

	void clear();
	void init(std::string fileName);
	void render(int frameNum);

	BVHObject();
	~BVHObject();
private:
	void renderJoint(Index joint, Pose pose);
	void renderBone(JH::Vector3d position0, JH::Vector3d position1);
};

class BVHObject2 {
	typedef int                 Index;
	typedef std::vector<double> Pose;

public:
	std::vector<keeper::Joint>   joints;
	std::vector<keeper::Channel> channels;
	std::vector<Pose>    motion;

	std::map<std::string, Index> jointNameToIndex;

	double       interval;
	int          nFrames;     // equal to motion.size()
	int          nChannels;   // equal to channels.size();
	bool         ready;
	std::string  name;

	bool         renderingJoint;
	bool         renderingBone;

	void clear();
	void init(std::string fileName);
	void render(int frameNum);

	BVHObject2();
	~BVHObject2();
private:
	void renderJoint(Index joint, Pose pose);
	void renderBone(JH::Vector3d position0, JH::Vector3d position1);
};