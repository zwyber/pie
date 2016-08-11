//
// Created by Yvan on 10-8-2016.
//

#include "visuals.h"

namespace visuals{
    // Variable that is the number of pixels per physics length unit.
    double pixRatio = 25.0;
}

/*
 * Function to initialise a new window that can be drawn on.
 */
GLFWwindow* initNewWindow(int width, int height){

    //Most of the initNewWindow code is from http://www.opengl-tutorial.org/ where this library set originates from.
    GLFWwindow* window;

    ///// initializing the GLFW functionality.
    // If the initfailed (returns false) it will throw an error
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
    }

    ///// Set the property of windows:
    // Make windows fixed size
    glfwWindowHint(GLFW_RESIZABLE,GL_TRUE);

    ///// Create the actual window
    // Open a window and create its OpenGL context
    window = glfwCreateWindow( width, height, "!!! Space debris debugger window !!!", NULL, NULL);
    // Return error if window did not get created
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window.\n" );
        getchar();
        glfwTerminate();
    }
    // Set the working space of gl to window
    glfwMakeContextCurrent(window);

    ///// Initialize GLEW
    //return error if this failed.
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
    }

    ///// Toggle the input mode.
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Return the pointer to the window we created.
    return window;
}
/*
 * Function to draw a grid in the current context window
 */
void drawGrid(int stepSize){
    // give stepSize in px

    // Get the window the Grid will be drawn to.
    GLFWwindow* CurrentWindow = glfwGetCurrentContext();

    // Declare integers the getwindowssize can write to.
    int Width;
    int Height;

    // get width and height of the open window
    glfwGetWindowSize(CurrentWindow, &Width, &Height);

    // Start Drawing lines
    glBegin(GL_LINES);
    // set line colours
    glColor3d(0.75, 0.75, 0.75);
    // Iterate over half screen width and draw lines from middle to both left and right
    // (each line is drawn from bottom to top)
    for(int i=0;i<=Width/2;i+=stepSize)
    {
        // Place drawing cursor, right
        glVertex2d((double)i*2.0/Width,-1.0);
        // Draw line from cursor to point:
        glVertex2d((double)i*2.0/Width,1.0);

        // Place drawing cursor, left
        glVertex2d((double)-i*2.0/Width,-1.0);
        // Draw line from cursor to point:
        glVertex2d((double)-i*2.0/Width,1.0);
    }
    // Iterate over half screen height and draw lines from middle to both top and bottom
    // (each line is drawn from left to right)
    for(int i=0;i<=Height/2;i+=stepSize)
    {
        // Place drawing cursor, up
        glVertex2d(-1.0,(double)i*2.0/Height);
        // Draw line from cursor to point:
        glVertex2d(1.0,(double)i*2.0/Height);

        // Place drawing cursor, down
        glVertex2d(-1.0,(double)-i*2.0/Height);
        // Draw line from cursor to point:
        glVertex2d(1.0,(double)-i*2.0/Height);
    }
    // end drawing lines.
    glEnd();
}
/*
 * Function to draw a circle on screen existing out of num_segments-2 triangles, centered on pos and having radius r.
 * Because r is normalized to height, we need screen screenWtHratio to get the x positions
 * The function is a modification of http://slabode.exofire.net/circle_draw.shtml#
 */
void drawFilledCircle(vec2d &pos, GLdouble &r, int num_segments, GLdouble &screenWtHRatio, std::array<double,4> Colour){
    // !! r needs to be normalized to screen HEIGHT !!
    //// Define constants/parameters for calculations
    // Get the stepsize angle of a complete circle
    const GLdouble phi = 2.0 / double(num_segments) * PI ;
    // precalculate the sine and cosine of angle phi
    const GLdouble c = cos(phi);
    const GLdouble s = sin(phi);
    // Define a parameter to store x to in the calculation
    GLdouble t;
    // Define relative position of the drawing cursor
    GLdouble x = r;//we start at angle = 0
    GLdouble y = 0;

    //// start drawing
    // Start a surface (triangle fan) (Triangle n is drawn with points n+1, n+2 and 1)
    glBegin(GL_TRIANGLE_FAN);
    // Set colour of the triangle fan.
    glColor4d(Colour[0], Colour[1], Colour[2], Colour[3]);

    // !!! In case we wish to use textures it's important to include the following line:
    glVertex2d(pos[0], pos[1]);
    for(int ii = 0; ii < num_segments; ii++)
    {
        // Add a drawing point to the circle outline
        glVertex2d(x/screenWtHRatio + pos[0], y + pos[1]);//output vertex

        // Apply the rotation matrix, to rotate around the circle centre so this point can be drawn next iteration.
        // Rotation matrix = [cos(phi), -sin(phi); sin(phi) cos(phi)]
        t = x;
        x = c * x - s * y;
        y = s * t + c * y;
    }
    // !!! In case we wish to use textures it's important to include the following line:
    glVertex2d(x/screenWtHRatio + pos[0], y + pos[1]);
    // end drawing
    glEnd();
}
/*
 * Function to draw all Objects from an object list to the current window
 */
void drawObjectList(std::vector<Object*> &objects){
    // get the current window pointer to get its size so we can normalize the universe size to pixel size.
    GLFWwindow* CurrentWindow = glfwGetCurrentContext();
    // Create parameters where glfwGetWindowSize can write to.
    int Width;
    int Height;
    glfwGetWindowSize(CurrentWindow, &Width, &Height);
    // Create the WtHratio for drawFilledCircle
    GLdouble WtHratio = (GLdouble)Width/Height;
    for(int ii = 0; ii < objects.size(); ii++){
        // Normalize the radius from universe to height [-1, 1];
        GLdouble radius = visuals::pixRatio*2.0*objects[ii]->get_radius()/Height;
        vec2d position = objects[ii]->get_position();
        // Normalize the position from universe to [-1, 1];
        position[0] *= visuals::pixRatio*2.0/Width;
        position[1] *= visuals::pixRatio*2.0/Height;
        // Draw the circle at the position
        drawFilledCircle(position, radius, 100, WtHratio, objects[ii]->get_colour());
    }
}