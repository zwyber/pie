//
// Created by Yvan on 10-8-2016.
//

#include "visuals.h"

/*
 * Function to initialise a new window that can be drawn on.
 */
Window::Window(Universe* uni, double pixelRatio){
    pixRatio = pixelRatio;
    winHeight = uni->height*pixRatio;
    winWidth = uni->width*pixRatio;
    winWtHratio = (double)winWidth/winHeight;
    boundUniverse = uni;
    activeFlag = vis::AUTO_SIZE_UNIVERSE;
    stdInitWindow();
}

Window::Window(int width, int height, Universe* uni, double pixelRatio, const unsigned flag){
    winHeight = height;
    winWidth = width;
    winWtHratio = (double)width/height;
    pixRatio = pixelRatio;
    boundUniverse = uni;
    if(flag == vis::AUTO_SIZE_UNIVERSE && boundUniverse!=NULL){
        boundUniverse->resize(width,height);
    }
    activeFlag = flag;
    stdInitWindow();
}

void Window::stdInitWindow(){

    //Most of this code is from http://www.opengl-tutorial.org/ where this library set originates from.
    if(boundUniverse!=NULL){
        uniToWinRatio = {boundUniverse->width*pixRatio/winWidth, boundUniverse->height*pixRatio/winHeight};
    }

    ///// initializing the GLFW functionality.
    // If the initfailed (returns false) it will throw an error
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
    }

    ///// Set the property of windows:
    // Make windows fixed size
    if(activeFlag == 3){
        glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
    }else{
        glfwWindowHint(GLFW_RESIZABLE,GL_TRUE);
    }

    ///// Create the actual window
    // Open a window and create its OpenGL context
    GLFWpointer = glfwCreateWindow(winWidth, winHeight, "!!! Space debris debugger window !!!", NULL, NULL);


    // Return error if window did not get created
    if( GLFWpointer == NULL ){
        fprintf( stderr, "Failed to open GLFW window.\n" );
        getchar();
        glfwTerminate();
    }

    // Set the working space of gl to window
    glfwMakeContextCurrent(GLFWpointer);

    ///// Initialize GLEW
    // Return error if this failed.
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
    }

    ///// Toggle the input mode.
    glfwSetInputMode(GLFWpointer, GLFW_STICKY_KEYS, GL_TRUE);

    // Couple the pointer to this Window to that of the GLFW window pointer
    // This will be used in WindowResizeStaticCallback (see .h class definition)
    glfwSetWindowUserPointer(GLFWpointer, this);

    // Call th function WindowResizeStaticCallback (which refers to window_size_callback)
    // when the window gets rescaled.
    glfwSetWindowSizeCallback(GLFWpointer, WindowResizeStaticCallback);
}

void Window::pace_frame() {

    // Check how much time we have left
    double delta = glfwGetTime() - lastTime;
    if (delta < 1/fps) {
        usleep(unsigned((1/fps - delta)*1E6));

        if (delta/(1.0/fps) > 0.75) {
            //std::cerr << "WARNING at 75% of CPU time per frame" << std::endl;
        }
    }
    else{
        // std::cerr << "CANNOT REACH TARGET FPS" << std::endl;
    }

    lastTime = glfwGetTime();

}

void Window::bindUniverse(Universe *uni) {
    boundUniverse = uni;
    uniToWinRatio = {boundUniverse->width*pixRatio/winWidth, boundUniverse->height*pixRatio/winHeight};
    window_size_callback(winWidth, winHeight);
}

void Window::changeResizeFlag(unsigned flag){
    if(boundUniverse == NULL){
        std::cerr << "[WARN]: tried to change windows resize flag while no Universe was bound. Did not recalculate ratio of universe to window" << std::endl;
    }else{
        uniToWinRatio = {boundUniverse->width*pixRatio/winWidth, boundUniverse->height*pixRatio/winHeight};
    }
    if(activeFlag == vis::NO_RESIZE && flag != vis::NO_RESIZE){
        std::cerr << "[WARN]: Cannot change property 'GLFW_RESIZABLE' to 'GL_TRUE' after initialization." << std::endl;
    }else if(flag == vis::NO_RESIZE){
        std::cerr << "[WARN]: Cannot change property 'GLFW_RESIZABLE' to 'GL_FALSE' after initialization." << std::endl;
    }
    activeFlag = flag;
    window_size_callback(winWidth,winHeight);
}

/*
 * Function to draw a grid in the current context window
 */
void Window::drawGrid(int stepSize){
    // give stepSize in px

    // Start Drawing lines
    glBegin(GL_LINES);
    // set line colours
    glColor3d(0.75, 0.75, 0.75);
    // Iterate over half screen width and draw lines from middle to both left and right
    // (each line is drawn from bottom to top)
    for(int i=0;i<=winWidth/2;i+=stepSize)
    {
        // Place drawing cursor, right
        glVertex2d((double)i*2.0/winWidth,-1.0);
        // Draw line from cursor to point:
        glVertex2d((double)i*2.0/winWidth,1.0);

        // Place drawing cursor, left
        glVertex2d((double)-i*2.0/winWidth,-1.0);
        // Draw line from cursor to point:
        glVertex2d((double)-i*2.0/winWidth,1.0);
    }
    // Iterate over half screen height and draw lines from middle to both top and bottom
    // (each line is drawn from left to right)
    for(int i=0;i<=winHeight/2;i+=stepSize)
    {
        // Place drawing cursor, up
        glVertex2d(-1.0,(double)i*2.0/winHeight);
        // Draw line from cursor to point:
        glVertex2d(1.0,(double)i*2.0/winHeight);

        // Place drawing cursor, down
        glVertex2d(-1.0,(double)-i*2.0/winHeight);
        // Draw line from cursor to point:
        glVertex2d(1.0,(double)-i*2.0/winHeight);
    }
    // end drawing lines.
    glEnd();
}
/*
 * Function to draw a circle on screen existing out of num_segments-2 triangles, centered on pos and having radius r.
 * Because r is normalized to height, we need screen screenWtHratio to get the x positions
 * The function is a modification of http://slabode.exofire.net/circle_draw.shtml#
 */
void Window::drawFilledCircle(vec2d &pos, GLdouble &r, int num_segments, std::array<double,4> Colour){
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

    // !!! In case we wish to use textures it's important to include the following lines:
    //glVertex2d(pos[0], pos[1]);
    //glVertex2d(x/winWtHratio + pos[0], y + pos[1]);

    for(int ii = 0; ii < num_segments; ii++)
    {
        // Apply the rotation matrix, to rotate around the circle centre so this point can be drawn.
        // Rotation matrix = [cos(phi), -sin(phi); sin(phi) cos(phi)]
        t = x;
        x = c * x - s * y;
        y = s * t + c * y;

        // Add a drawing point to the circle outline
        glVertex2d(x/winWtHratio + pos[0], y + pos[1]);//output vertex
    }
    // end drawing
    glEnd();
}
/*
 * Function to draw all Objects from an object list to the current window
 */
void Window::drawObjectList(std::vector<Object*> &objects){

    for(int ii = 0; ii < objects.size(); ii++){
        // Normalize the radius from universe to height [-1, 1];
        GLdouble radius = pixRatio*2.0*objects[ii]->get_radius()/winHeight;
        vec2d position = objects[ii]->get_position();
        // Normalize the position from universe to [-1, 1];
        position[0] *= pixRatio*2.0/winWidth;
        position[1] *= pixRatio*2.0/winHeight;
        // Draw the circle at the position
        drawFilledCircle(position, radius, 100, objects[ii]->get_colour());
    }
}
/*
 * Draws a redbox to the middle of the screen.
 * Uses Universe scale!!!
 */
void Window::drawBox(double Width, double Height){

    glBegin(GL_LINE_LOOP);
    glColor4d(1.0,0,0,1.0);
    Width *= pixRatio/winWidth;
    Height *= pixRatio/winHeight;
    glVertex2d(Width, Height);
    glVertex2d(-Width, Height);
    glVertex2d(-Width, -Height);
    glVertex2d(Width, -Height);
    glEnd();
}
/*
 * Working function that is called when the window is resized.
 */
void Window::window_size_callback(int width, int height){
    // Update the parameters to fit those of the window.
    winHeight = height;
    winWidth = width;
    winWtHratio = (double)width/height;
    // Update GL's canvas to fit the window.
    // Some changes to the universe based on user preferences (activeFlag)
    switch(activeFlag) {
        case vis::FIXED_SIZE_UNIVERSE:
            break;
        case vis::AUTO_SIZE_UNIVERSE:
            boundUniverse->resize(width / pixRatio, height / pixRatio);
            break;
        case vis::PROP_SIZE_UNIVERSE:
        case vis::NO_RESIZE:
            boundUniverse->resize(uniToWinRatio[0] * width / pixRatio , uniToWinRatio[1] * height / pixRatio);
            break;
        case vis::ZOOM_UNIVERSE:
            pixRatio = uniToWinRatio[0]*width /boundUniverse->width;
           if(pixRatio > uniToWinRatio[1]*height/ boundUniverse->height){
                pixRatio = uniToWinRatio[1]*height/ boundUniverse->height;
           }
            break;
    }
    glViewport(0,0, width, height);
}

vec2d Window::px_to_length(vec2d px) {
    vec2d l = {0};

    l[0] = (px[0] - (winWidth/2.0)) / pixRatio;
    l[1] = (-px[1] + (winHeight/2.0)) / pixRatio;

    return l;
}

vec2d Window::length_to_px(vec2d length) {

    // It no works yet :(
    std::cerr << "length_to_px not implemented yet";

    return length;
}