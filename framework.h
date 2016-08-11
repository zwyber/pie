//
// Created by paul on 8/2/16.
//

#ifndef PIE_GITHUB_FRAMEWORK_H
#define PIE_GITHUB_FRAMEWORK_H

#ifndef PI
#define PI 3.14159265359;
#endif


// Standard libraries
#include<vector>
#include<array>
#include<iostream>
#include<map>
#include<cmath>
#include<fstream>
#include<cstdlib>

// Type definitions
typedef std::array<double, 2> vec2d;
typedef std::array<vec2d, 2> pos_vel;
// Own libraries
#include "vecmath.h"
#include "objects.h"
#include "visuals.h"

#endif //PIE_GITHUB_FRAMEWORK_H
