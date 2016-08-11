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

// Draws a grid in the current active window, the stepsize is the px size of the grid
void DrawGrid(int stepSize);

// Draws circles. Needs a position for the centre, a radius (normalized to screen height), number of points used to
// define the circle, screenratio (so you actually get circles and not ellipses) and finally it needs a colour
// consisting of {Red, Green, Blue, Alpha}.
void drawFilledCircle(vec2d &pos, GLdouble &r, int num_segments, GLdouble &screenWtHRatio, std::array<double,4> Colour);

// Goes through the object list and draws all objects to the current active window
void drawObjectList(std::vector<Object*> &objects, double Uwidth, double Uheight);

#include "visuals.cpp"

#endif //TUTORIALS_VISUALS_H
