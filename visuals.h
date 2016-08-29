//
// Created by Yvan on 10-8-2016.
//

#ifndef PIE_GITHUB_FRAMEWORK_H

#include "framework.h"

#endif

#ifndef TUTORIALS_VISUALS_H
#define TUTORIALS_VISUALS_H

namespace vis{
    const unsigned FIXED_SIZE_UNIVERSE = 0;
    const unsigned AUTO_SIZE_UNIVERSE = 1;
    const unsigned PROP_SIZE_UNIVERSE = 2;
    const unsigned NO_RESIZE = 3;
    const unsigned ZOOM_UNIVERSE = 4;
}

class Window{
private:
    // Keep the following parameters protected but shared in the draw functions
    int winHeight;
    int winWidth;
    double winWtHratio;

    //// properties in order to resolve universe and window conversions
    // Currently only a single universe can be bound to a window, if needed this can be easily changed to a vector
    Universe* boundUniverse;
    unsigned activeFlag;

    //
    vec2d uniToWinRatio;
    void stdInitWindow();
public:
    // Constructor that makes a window use ready.
    Window(Universe* uni, double pixRatio);
    Window(int width, int height, Universe* uni, double pixelRatio,const unsigned flag);

    double fps = 60;
    double lastTime = glfwGetTime();

    void bindUniverse(Universe* uni);
    void changeResizeFlag(unsigned flag);
    // The pointer to the actual GLFWwindow this class controls
    GLFWwindow* GLFWpointer;

    // The function that gets called (eventually) when we rescale the window
    void window_size_callback(int width, int height);

    // Variable that is the number of pixels per physics length unit.
    double pixRatio = 25.0;
// Draws a grid in the current active window, the stepsize is the px size of the grid
    void drawGrid(int stepSize);

// Draws circles. Needs a position for the centre, a radius (normalized to screen height), number of points used to
// define the circle, screenratio (so you actually get circles and not ellipses) and finally it needs a colour
// consisting of {Red, Green, Blue, Alpha}.
    void drawFilledCircle(vec2d &pos, GLdouble &r, int num_segments, std::array<double, 4> Colour);

// Goes through the object list and draws all objects to the current active window
    void drawObjectList(std::vector<Object *> &objects);

    // Draws a simple red box in the middle of the screen.
    void drawBox(double Width, double Height);

    void drawFT_Bitmap(FT_Bitmap* image, int xleft, int ytop);

    // For frame pacing
    void pace_frame();

    // Convert from px to universe length scale
    vec2d px_to_length (vec2d px);
    vec2d length_to_px (vec2d length);

    vec2d windowSize();
    // Too avoid call back issues the method of http://gamedev.stackexchange.com/questions/58541/how-can-i-associate-a-key-callback-with-a-wrapper-class-instance
    // has been used to define functions in this class.

    // This function format makes it appear to GLFWcallback functions (as it now does for anything else) as if the function was global.
    // As the callback functions are incapable of interpreting functions from a class
    static void WindowResizeStaticCallback(GLFWwindow* win, int width, int height){
        // We need to know which Window actually called which we can do as we coupled the GLFW user pointer to this Window pointer.
        Window *window = static_cast<Window*>(glfwGetWindowUserPointer(win));
        // now we got our Window pointer and we can call functions from WITHIN the class.
        window->window_size_callback(width, height);
    }
};

#include "visuals.cpp"

#endif //TUTORIALS_VISUALS_H
