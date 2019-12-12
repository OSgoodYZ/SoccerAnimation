//jihun test
#pragma once

#include <vector>
#include <string>
#include "Vector3.h"
//#include <Eigen/Dense>

//using namespace Eigen;
namespace kicker {
	class BVHObject;

	class Joint {
		typedef int Index;

	public:
		std::string          name;
		Index                index;
		Index                parent;        // if no parent, - 1
		std::vector<Index>   children;
		JH::Vector3d             offset;
		bool                 hasSite;
		JH::Vector3d             site;
		std::vector<Index>   channels;
	};
};


namespace keeper {
	class BVHObject2;

	class Joint {
		typedef int Index;

	public:
		std::string          name;
		Index                index;
		Index                parent;        // if no parent, - 1
		std::vector<Index>   children;
		JH::Vector3d             offset;
		bool                 hasSite;
		JH::Vector3d             site;
		std::vector<Index>   channels;
	};
};