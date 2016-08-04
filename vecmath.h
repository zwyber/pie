//
// Created by paul on 8/3/16.
//

#ifndef PIE_GITHUB_FRAMEWORK_H

#include "framework.h"

#endif

#ifndef PIE_GITHUB_MATH_H
#define PIE_GITHUB_MATH_H

// Vector functions
double dot (vec2d a, vec2d b);
vec2d sub (vec2d a, vec2d b);
vec2d add (vec2d a, vec2d b);
vec2d cmult (vec2d v, double c);
double len (vec2d a);
double len_squared (vec2d a);

#include "vecmath.cpp"

#endif //PIE_GITHUB_MATH_H
