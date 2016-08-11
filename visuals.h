//
// Created by Yvan on 10-8-2016.
//

#ifndef PIE_GITHUB_FRAMEWORK_H

#include "framework.h"

#endif

#ifndef TUTORIALS_VISUALS_H
#define TUTORIALS_VISUALS_H

// Standard initialization for a new window
// returns the pointer to that window
GLFWwindow* initNewWindow(int width, int height);
void DrawGrid(int stepSize);
void drawFilledCircle(vec2d &pos, GLdouble &r, int num_segments, GLdouble &screenWtHRatio, std::array<double,4> Colour);
void drawObjectList(std::vector<Object*> &objects, double Uwidth, double Uheight);

#include "visuals.cpp"

#endif //TUTORIALS_VISUALS_H
