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
#include<thread>
#include<unistd.h>
#include<string>

#ifndef PIE_ONLY_BACKEND
    // Used GL libraries
    #include <GL/glew.h>
    #include <glfw3.h>
    #include <glm/glm.hpp>
    #include <ft2build.h>
    #include "external/opengltut_common/shader.hpp"
    #include "external/opengltut_common/texture.hpp"
    #include FT_FREETYPE_H
#endif // PIE_ONLY_BACKEND

// Type definitions
typedef std::array<double, 2> vec2d;
typedef std::array<vec2d, 2> pos_vel;

#ifndef PIE_ONLY_BACKEND
    struct Character{
        GLuint textureID;
        glm::ivec2 Size;
        glm::ivec2 Bearing;
        FT_Pos Advance;
    };
#endif // PIE_ONLY_BACKEND

// Own libraries
#include "lib/vecmath.h"
#include "lib/simulation.h"

#ifndef PIE_ONLY_BACKEND
    #include "lib/visuals.h"
#endif // PIE_ONLY_BACKEND

#endif //PIE_GITHUB_FRAMEWORK_H
