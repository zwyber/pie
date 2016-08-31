//
// Created by paul on 8/3/16.
//

#include "vecmath.h"

double dot (vec2d a, vec2d b) {
    return (a[0] * b[0] + a[1] * b[1]);
}

vec2d sub (vec2d a, vec2d b) {
    vec2d r;
    r[0] = a[0] - b[0];
    r[1] = a[1] - b[1];

    return r;
}

vec2d add (vec2d a, vec2d b) {
    vec2d r;
    r[0] = a[0] + b[0];
    r[1] = a[1] + b[1];

    return r;
}

vec2d cmult (vec2d v, double c) {
    v[0] = v[0] * c;
    v[1] = v[1] * c;

    return v;
}

double len (vec2d a) {
    // Quick and dirty
    return std::sqrt( a[0]*a[0] + a[1]*a[1] );
}

double len_squared (vec2d a) {
    return ( a[0]*a[0] + a[1]*a[1] );
}
