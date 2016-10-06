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
#include<sstream>
#include<cstdlib>
#include<ctime>
#include<chrono>
#include<unistd.h>
#include<string>

// Used GL libraries
#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <ft2build.h>
#include "external/opengltut_common/shader.hpp"
#include "external/opengltut_common/texture.hpp"
#include FT_FREETYPE_H
//#include "external/ahrs-visualizer/png_texture.cpp"


// Type definitions
typedef std::array<double, 2> vec2d;
typedef std::array<vec2d, 2> pos_vel;

struct Character{
    GLuint textureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    FT_Pos Advance;
};

// Own libraries
#include "lib/vecmath.h"
#include "lib/simulation.h"
#include "lib/visuals.h"

#endif //PIE_GITHUB_FRAMEWORK_H
