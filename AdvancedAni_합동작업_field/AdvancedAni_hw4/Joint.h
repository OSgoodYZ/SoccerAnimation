//jihun test
#pragma once

#include <vector>
#include <string>
#include "Vector3.h"
//#include <Eigen/Dense>

//using namespace Eigen;
using namespace JH;
class BVHObject;

class Joint {
    typedef int Index;
    
public:
    std::string          name;
    Index                index;
    Index                parent;        // if no parent, - 1
    std::vector<Index>   children;
    Vector3d             offset;
    bool                 hasSite;
    Vector3d             site;
    std::vector<Index>   channels;
};
