//
// Created by Yvan on 10-8-2016.
//

#include "visuals.h"

/*
 * Functions to initialise a new window that can be drawn on.
 */
// Generate a window that fits the universe
Window::Window(Universe* uni, double pixelRatio){
    pixRatio = pixelRatio;
    winHeight = uni->height*pixRatio;           // Set window to fit universe with current pixRatio
    winWidth = uni->width*pixRatio;
    winWtHratio = (double)winWidth/winHeight;   // Precalculate winWtHratio for later use
    boundUniverse = uni;                        // bind the universe to this window
    activeFlag = vis::AUTO_SIZE_UNIVERSE;   // Set flag to auto size universe
    stdInitWindow();    // use standard initiation
}
// Generate a window of width by height without a universe bound to it
Window::Window(int width, int  height, const unsigned flag){
    winHeight = height;
    winWidth = width;
    winWtHratio = (double)width/height;     // Precalculate winWtHratio for later use
    boundUniverse = NULL;                   // do not bind a universe to the current window
    activeFlag = flag;  // pass flag
    stdInitWindow();   // use standard initiation
}
// Generate a window of width by height with a bound universe
Window::Window(int width, int height, Universe* uni, double pixelRatio, const unsigned flag){
    winHeight = height;
    winWidth = width;
    winWtHratio = (double)width/height;     // Precalculate winWtHratio for later use
    pixRatio = pixelRatio;
    boundUniverse = uni;                    // Bind the universe to this window
    if(flag == vis::AUTO_SIZE_UNIVERSE && boundUniverse!=NULL){
        boundUniverse->resize(width,height);    // Resize universe to fit this window if AUTO_SIZE is on
    }
    activeFlag = flag;  // pass flag
    stdInitWindow();   // use standard initiation
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

    // Set the working space of gl to this window
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

    // Set a blend modus based on the alpha channel
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // This function means it adds the colours of the background multiplied by 1-alpha of overlay to the overlay;

    // Couple the pointer to this Window to that of the GLFW window pointer
    // This will be used in WindowResizeStaticCallback (see .h class definition)
    glfwSetWindowUserPointer(GLFWpointer, this);

    // Call th function WindowResizeStaticCallback (which refers to window_size_callback)
    // when the window gets rescaled.
    glfwSetWindowSizeCallback(GLFWpointer, WindowResizeStaticCallback);
    // Give the curser position an initial value
    cursorPos = {0,0};
    // Call the function cursor_position_callback when the mouse is moved (this refers to the overloaded function in class Window)
    glfwSetCursorPosCallback(GLFWpointer, cursor_position_callback);

}

void Window::pace_frame() {

    // Check how much time we have left
    double delta = glfwGetTime() - lastTime;
    if (delta < 1/fps) {
        // Pause program for the time that is left over
        usleep(unsigned((1/fps - delta)*1E6));

        // Debug message:
        /*
        if (delta/(1.0/fps) > 0.75) {
            std::cerr << "WARNING at 75% of CPU time per frame" << std::endl;
        }
         */
    }
    // Debug message:
    /*
    else{
        std::cerr << "CANNOT REACH TARGET FPS" << std::endl;
    }
     */
    lastTime = glfwGetTime();  // store current time for next iteration
}

void Window::bindUniverse(Universe *uni) {
    boundUniverse = uni;    // pass universe pointer
    // Apply necessary changes to universe
    if(boundUniverse!=NULL) {
        if (activeFlag == vis::AUTO_SIZE_UNIVERSE) {
            boundUniverse->resize(winWidth, winHeight); // Fit universe to window if AUTO_SIZE is on
        }
        // store universe size to window size ratio.
        uniToWinRatio = {boundUniverse->width * pixRatio / winWidth, boundUniverse->height * pixRatio / winHeight};
        window_size_callback(winWidth, winHeight);  // Call the window size callback to process any necessary changes to bound universe
    }
}

void Window::Resize(int width, int height){
    // small safety feature to for external use
    if(width<1){
        width = winWidth;
    }
    if(height<1){
        height = winHeight;
    }
    // use window_size_callback to properly process;
    window_size_callback(width,height);
}

void Window::changeResizeFlag(unsigned flag){
    if(boundUniverse == NULL){
        std::cerr << "[WARN]: tried to change windows resize flag while no Universe was bound. Did not recalculate ratio of universe to window" << std::endl;
    }else{
        // reset the uniToWinRatio so the flag properties are bound to the current window/universe state
        uniToWinRatio = {boundUniverse->width*pixRatio/winWidth, boundUniverse->height*pixRatio/winHeight};
    }
    // Cannot change to or from NO_RESIZE
    if(activeFlag == vis::NO_RESIZE && flag != vis::NO_RESIZE){
        std::cerr << "[WARN]: Cannot change property 'GLFW_RESIZABLE' to 'GL_TRUE' after initialization of Window." << std::endl;
    }else if(flag == vis::NO_RESIZE){
        std::cerr << "[WARN]: Cannot change property 'GLFW_RESIZABLE' to 'GL_FALSE' after initialization of Window." << std::endl;
    }
    activeFlag = flag; // pass flag
    window_size_callback(winWidth,winHeight);   // Apply current possible changes to universe
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
//// Circle drawing without shaders
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
// Draws Objects from the bound universe
void Window::drawObjectList(CircleShader* circleShader) {
    if(this->boundUniverse==NULL){
        std::cerr << "[WARN]: could not drawobjectlist, bound universe is missing (NULL)" << std::endl;
    }else {
        this->drawObjectList(this->boundUniverse->objects, circleShader);   // pass the bounduniverse objects to the drawobjectlist function
    }
}

void Window::drawObjectList(std::vector<Object*> &objects, CircleShader* circleShader){
    // If there is no shader this function uses the drawFilledCircle function defined above else it'll use the shader
    if(circleShader == NULL) {
        for (int ii = 0; ii < objects.size(); ii++) {
            // Normalize the radius from universe to height [-1, 1];
            GLdouble radius = pixRatio * 2.0 * objects[ii]->radius / winHeight;
            vec2d position = objects[ii]->position;
            // Normalize the position from universe to [-1, 1];
            position[0] *= pixRatio * 2.0 / winWidth;
            position[1] *= pixRatio * 2.0 / winHeight;
            // Draw the circle at the position
            drawFilledCircle(position, radius, std::sqrt(objects[ii]->radius) * 25, objects[ii]->colour);
        }
    }else{
        for (int ii = 0; ii < objects.size(); ii++) {
            // Normalize the radius from universe to height [-1, 1];
            double radius = pixRatio * 2.0 * objects[ii]->radius / winHeight;
            vec2d position = objects[ii]->position;
            // Normalize the position from universe to [-1, 1];
            position[0] *= pixRatio * 2.0 / winWidth;
            position[1] *= pixRatio * 2.0 / winHeight;
            // create a scale and translation matrix
            circleShader->transformationMatrix = {
                    radius/winWtHratio, 0, position[0],
                    0,             radius, position[1],
                    0,                  0,           1
            };
            // set shader colour
            circleShader->colour = objects[ii]->get_colour_glm();
            // Draw the circle at the position
            circleShader->draw();
        }
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
    if(boundUniverse != NULL) {
        switch (activeFlag) {
            case vis::FIXED_SIZE_UNIVERSE:
                break;
            case vis::AUTO_SIZE_UNIVERSE:
                boundUniverse->resize(width / pixRatio, height / pixRatio);
                break;
            case vis::PROP_SIZE_UNIVERSE:
            case vis::NO_RESIZE:
                boundUniverse->resize(uniToWinRatio[0] * width / pixRatio, uniToWinRatio[1] * height / pixRatio);
                break;
            case vis::ZOOM_UNIVERSE:
                pixRatio = uniToWinRatio[0] * width / boundUniverse->width;
                if (pixRatio > uniToWinRatio[1] * height / boundUniverse->height) {
                    pixRatio = uniToWinRatio[1] * height / boundUniverse->height;
                }
                break;
        }
    }
    // Resize 'drawing canvas' to screen size
    glViewport(0,0, width, height);
}
void Window::cursor_position_callback(double xpos, double ypos){
    // Store the cursor position in normalized coordinates (-1 to 1)
    cursorPos = {2*xpos/winWidth -1, -2*ypos/winHeight +1};
}
vec2d Window::windowSize() {return {(double)winWidth,(double)winHeight};}

vec2d Window::cursorPosition(){
    return cursorPos;
}

/*
 * Shaders section
 */
Shader::Shader(const char *vertexShader, const char *fragmentShader, const char *vertexName, const char *tMatrixName) {
    transformationMatrix = glm::mat3(1.0f);                         // set to a identity matrix
    programID = LoadShaders(vertexShader, fragmentShader);          // use LoadShaders to build GLSL program and get the programID
    vertexPositionID = glGetAttribLocation(programID, vertexName);  // get the location of the vertexName parameter

    if(tMatrixName!=NULL){
        tMatrixID = glGetUniformLocation(programID, tMatrixName);   // If there is a matrix location provided add this parameter
        tMatrixOn = true;
    } else{
        tMatrixOn = false;
    }

    static const GLfloat vertices[] = {
            -1.0f,-1.0f,
            -1.0f,1.0f,
            1.0f,1.0f,
            1.0f,-1.0f
    };  // Fill the buffer with vertices for a drawcommand to fill the screen.

    vertexCount = sizeof(vertices)/(sizeof(GLfloat)*2); // Store amount off vertices in buffer

    glGenBuffers(1, &vertexBuffer);                                             // Generate a buffer and store its location ID in vertexBuffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);                                // Make the vertex buffer active (current)
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);  // Write the vertices to bufferdata vertexBuffer and set the access type of this buffer
}
Shader::~Shader(){
    glDeleteBuffers(1, &vertexBuffer);  // Free the vertexBuffer
    glDeleteProgram(programID);         // Delete the shader program
}
void Shader::tMatrixReset(){
    transformationMatrix = glm::mat3(1.0f); // Set transformation matrix to Identity matrix
}
void Shader::tMatrixRotate(GLfloat angle) {
    const GLfloat c = cos(angle);
    const GLfloat s = sin(angle);
    transformationMatrix *= glm::mat3(
            c, -s, 0,
            s,  c, 0,
            0,  0, 1
    );  // Multiply the transformation matrix with a rotation matrix
}
void Shader::tMatrixScale(vec2d scale) {
    transformationMatrix *= glm::mat3(
            scale[0], 0, 0,
            0, scale[1], 0,
            0,        0, 1
    ); // Multiply the transformation matrix with a scaling matrix
}
void Shader::tMatrixTranslate(vec2d position) {
    transformationMatrix *= glm::mat3(
            1, 0, position[0],
            0, 1, position[1],
            0, 0, 1
    );
}
void Shader::setNewVertices(GLuint arraySize, const GLfloat *vertexArray) {
    vertexCount = arraySize/2;  // Safe the vertex count
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);                                           // Make the buffer active
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*arraySize, vertexArray, GL_STATIC_DRAW); // Rewrite the buffer with new vertices
}
void Shader::draw(){
    glUseProgram(programID);    // Activate the Shader Program as current shader

    if(tMatrixOn) glUniformMatrix3fv(tMatrixID, 1, GL_FALSE, &transformationMatrix[0][0]);  // Pass the transformation matrix to GLSL program if available

    glEnableVertexAttribArray(vertexPositionID);    // Enable the GLSL attribute vertice positions
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(
            vertexPositionID,  // The vertex attribute
            2,                            // size
            GL_FLOAT,                     // type
            GL_FALSE,                     // normalized?
            0,                            // stride
            (void*)0                      // array buffer offset (incase you have multiple attribute elements in one buffer) not used
    );

    glDrawArrays(GL_TRIANGLE_FAN, 0,vertexCount);   // Draw the stored buffer

    glDisableVertexAttribArray(vertexPositionID);   // Disable the attribute
    glUseProgram(0);                                // Reset program to none
}

TextureShader::TextureShader(GLuint texture_) : Shader("shaders/texture.glvs", "shaders/texture.glfs", "PositionVec", "MVP"){ // Build the inherited class with the following constructor parameters
    texture = texture_;                                         // Pass the texture location
    vertexUVID = glGetAttribLocation(programID, "vertexUV");    // Get the location of the UV buffer from the GLSL
    textureID  = glGetUniformLocation(programID, "tex");        // Get the location of texture sampler from GLSL

    static const GLfloat texcoords[] = {
            0.0f,1.0f,
            0.0f,0.0f,
            1.0f,0.0f,
            1.0f,1.0f
    };  // Store the UV buffer all most outer points from the texture matched to the UV points in Shader class

    glGenBuffers(1, &uvBuffer);                 // Generate a buffer for and store the location ID in uvBuffer
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);    // Make the buffer the current working buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);    // Write the UV coordinates to the buffer

}
void TextureShader::setNewUVCoordinates(GLuint arraySize, const GLfloat *uvArray) {
    unsigned uvCount = arraySize/2;     // Amount of UV coordinates
    if(uvCount != vertexCount){
        if(uvCount > vertexCount) {
            std::cout << "[Warn]: in setNewUVCoordinates, there were given too many coordinates. Cut UV off at "
                      << vertexCount
                      << ". If you want to set UV and vertices at the same time, please use supply both pointers."
                      << std::endl;
        }else {
            std::cout << "[Warn]: in setNewUVCoordinates, there were given too many coordinates. Cut vertices off at "
                      << uvCount
                      << ". If you want to set UV and vertices at the same time, please use supply both pointers."
                      << std::endl;
            vertexCount = arraySize / 2;
        }
    } // Only print a warning if there is a mismatch between the UV and vertex coordinates, (drawing will not work correctly until the other the mismatch is Fixed!)
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);    // Make the buffer current (edit it)
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertexCount*2, uvArray, GL_STATIC_DRAW);  // Write the UV Array to the buffer
}
void TextureShader::setNewUVCoordinates(GLuint arraySize, const GLfloat *uvArray, const GLfloat *vertexArray){
    // Set both UV and vertex coordinates
    vertexCount = arraySize/2;  // update the vertexcount

    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);    // Make UV buffer current
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*arraySize, uvArray, GL_STATIC_DRAW); // Write the UV array to the UV buffer

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);    // Make the vertex buffer current
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*arraySize, vertexArray, GL_STATIC_DRAW);  // Write the vertex array to the vertexbuffer
}
TextureShader::~TextureShader(){
    glDeleteBuffers(1, &uvBuffer);      // Remove the buffer reservation
    glDeleteTextures(1, &textureID);    // Remove the texture reservation
}
void TextureShader::draw(unsigned texNum){
    glUseProgram(programID);    // Activate the GLSL program
    glUniformMatrix3fv(tMatrixID, 1, GL_FALSE, &transformationMatrix[0][0]);    // pass the trasnformation matrix to the program

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glUniform1i(textureID, 0);  // Give the texture sampler (from the GLSL program) the location  to sample: Texture unit 0

    glEnableVertexAttribArray(vertexPositionID);    // Enable the GLSL vertex attribute
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);    // Make the vertex buffer current
    // Bind the vertex position of the GLSL program to the vertex buffer.
    glVertexAttribPointer(
            vertexPositionID,  // The vertex attribute
            2,                            // size per vertex
            GL_FLOAT,                     // type
            GL_FALSE,                     // normalized?
            0,                            // stride
            (void*)0                      // array buffer offset (incase you have multiple attribute elements in one buffer) not used
    );

    glEnableVertexAttribArray(vertexUVID);      // Enable the GLSL UV attribute
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);    // make the UV buffer current
    // Bind the UV coordinates of the GLSL program to the uv Buffer
    glVertexAttribPointer(
            vertexUVID,                   // The UV attribute
            2,                            // size per UV coordinate
            GL_FLOAT,                     // type
            GL_FALSE,                     // normalized?
            0,                            // stride
            (void*)0                      // array buffer offset (incase you have multiple attribute elements in one buffer) not used
    );

    glDrawArrays(GL_TRIANGLE_FAN, texNum*4,4); // Draw a part of the texture (part number texNum)

    glDisableVertexAttribArray(vertexPositionID);   // Disable the vertex position in the program
    glDisableVertexAttribArray(vertexUVID);         // Disable the UV coordinates in the program
    glUseProgram(0);    // Disable the Program and set it the used program to none
}

CircleShader::CircleShader(glm::vec4 colour_) : Shader("shaders/circle.glvs", "shaders/circle.glfs", "inPosition", "projection"){ // Build the inherited class with the following constructor parameters
    colour = colour_;   // Pass a standard colour
    vertexUVID = glGetAttribLocation(programID, "inTexcoord");  // Get the UV location in the GLSL program
    colourID  = glGetUniformLocation(programID, "disc_color");  // Get the colour location in the GLSL program

    static const GLfloat texcoords[] = {
            0.0f,0.0f,
            0.0f,1.0f,
            1.0f,1.0f,
            1.0f,0.0f
    };  // Store the UV buffer all most outer points from the texture matched to the UV points in Shader class

    glGenBuffers(1, &uvBuffer);                 // Generate a buffer for and store the location ID in uvBuffer
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);    // Make the buffer the current working buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);    // Write the UV coordinates to the buffer

}
void CircleShader::setNewUVCoordinates(GLuint arraySize, const GLfloat *uvArray) {
    unsigned uvCount = arraySize/2;     // Amount of UV coordinates
    if(uvCount != vertexCount){
        if(uvCount > vertexCount) {
            std::cout << "[Warn]: in setNewUVCoordinates, there were given too many coordinates. Cut UV off at "
                      << vertexCount
                      << ". If you want to set UV and vertices at the same time, please use supply both pointers."
                      << std::endl;
        }else {
            std::cout << "[Warn]: in setNewUVCoordinates, there were given too many coordinates. Cut vertices off at "
                      << uvCount
                      << ". If you want to set UV and vertices at the same time, please use supply both pointers."
                      << std::endl;
            vertexCount = arraySize / 2;
        }
    } // Only print a warning if there is a mismatch between the UV and vertex coordinates, (drawing will not work correctly until the other the mismatch is Fixed!)
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);    // Make the buffer current (edit it)
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertexCount*2, uvArray, GL_STATIC_DRAW);  // Write the UV Array to the buffer
}
void CircleShader::setNewUVCoordinates(GLuint arraySize, const GLfloat *uvArray, const GLfloat *vertexArray){
    // Set both UV and vertex coordinates
    vertexCount = arraySize/2;  // update the vertexcount

    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);    // Make UV buffer current
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*arraySize, uvArray, GL_STATIC_DRAW); // Write the UV array to the UV buffer

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);    // Make the vertex buffer current
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*arraySize, vertexArray, GL_STATIC_DRAW);  // Write the vertex array to the vertexbuffer
}
CircleShader::~CircleShader(){
    glDeleteBuffers(1, &uvBuffer);      // Remove the buffer reservation
}
void CircleShader::draw(){
    glUseProgram(programID);    // Activate the GLSL program
    glUniformMatrix3fv(tMatrixID, 1, GL_FALSE, &transformationMatrix[0][0]);    // pass the trasnformation matrix to the program

    glUniform4f(colourID, colour.r, colour.g, colour.b, colour.a);  // Pass the colour vector

    glEnableVertexAttribArray(vertexPositionID);    // Enable the GLSL vertex attribute
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);    // Make the vertex buffer current
    // Bind the vertex position of the GLSL program to the vertex buffer.
    glVertexAttribPointer(
            vertexPositionID,  // The vertex attribute
            2,                            // size per vertex
            GL_FLOAT,                     // type
            GL_FALSE,                     // normalized?
            0,                            // stride
            (void*)0                      // array buffer offset (incase you have multiple attribute elements in one buffer) not used
    );

    glEnableVertexAttribArray(vertexUVID);      // Enable the GLSL UV attribute
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);    // make the UV buffer current
    // Bind the UV coordinates of the GLSL program to the uv Buffer
    glVertexAttribPointer(
            vertexUVID,                   // The UV attribute
            2,                            // size per UV coordinate
            GL_FLOAT,                     // type
            GL_FALSE,                     // normalized?
            0,                            // stride
            (void*)0                      // array buffer offset (incase you have multiple attribute elements in one buffer) not used
    );

    glDrawArrays(GL_TRIANGLE_FAN, 0,vertexCount);   // Draw the vertices (square) to fill with a circle

    glDisableVertexAttribArray(vertexPositionID);   // Disable the vertex position in the program
    glDisableVertexAttribArray(vertexUVID);         // Disable the UV coordinates in the program
    glUseProgram(0);    // Disable the Program and set it the used program to none
}

TextShader::TextShader(const char* trueTypePath, int numOfChars) : Shader("shaders/text.glvs", "shaders/text.glfs", "VertexPos", "projection"){ // Build the inherited class with the following constructor parameters
    if (FT_Init_FreeType(&ft))  // Initiate a freetype library to ft
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

    if (FT_New_Face(ft, trueTypePath, 0, &face))    // initiate a Face of the specified font to face
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

    pixSize = 72; // A pixel size to draw the fonts
    FT_Set_Pixel_Sizes(face, 0, pixSize);   // Set the size of the character to the Face
    colour = glm::vec4(1.0f);   // Store a white colour in colour
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction


    for (GLubyte c = 0; c < numOfChars; c++)
    {
        // Load character glyph (render it)
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        // Write the texture as single colour channel (alpha) from the bitmap buffer generated in the face.
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
        );

        // Set texture options (wrap around and scaling)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Now store character for later use
        Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x
        };
        Characters.insert(std::pair<GLchar, Character>(c, character)); // add a character char part
    }
    glBindTexture(GL_TEXTURE_2D, 0);    // remove the binding

    FT_Done_Face(face);     // Clear face resources
    FT_Done_FreeType(ft);   // Clear FreeType libray resources

    static const GLfloat texcoords[] = {
            0.0f,1.0f,
            0.0f,0.0f,
            1.0f,0.0f,
            1.0f,1.0f
    };    // Store the UV buffer all most outer points from the texture matched to the UV points in Shader class

    glGenBuffers(1, &uvBuffer);                 // Generate a buffer for and store the location ID in uvBuffer
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);    // Make the buffer the current working buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);    // Write the UV coordinates to the buffer

    vertexUVID = glGetAttribLocation(programID, "vertexUV");    // Get the location of the UV coordinates in the GLSL program
    textureID  = glGetUniformLocation(programID, "text");       // Get the location of the texture (character) in the GLSL program
    textColorID = glGetUniformLocation(programID, "textColor"); // Get the location of the colour in the GLSL program

}
TextShader::~TextShader(){
    glDeleteBuffers(1, &uvBuffer);  // Clear the buffer reservation
    for (map<char,Character>::iterator c = Characters.begin(); c != Characters.end(); c++){
        glDeleteTextures(1,&(*c).second.textureID); // Clear the texture reservation for each character
    }
}
void TextShader::draw(std::string text, vec2d position, unsigned alignment,vec2d screenDims, double height){

    glUseProgram(programID);    // activate the correct shader program
    glUniform3f(textColorID, colour.x, colour.y, colour.z); // pass the text colour
    glActiveTexture(GL_TEXTURE0);   // make texture unit 0 active
    glUniform1f(textureID, 0);      // make the GLSL texture sampler look at texture unit 0

    // Create a scale corresponding to the rendered scale and input height
    double yScale = 2 * height / pixSize;
    double xScale = yScale;

    if(screenDims[0] && screenDims[1])
        xScale *= screenDims[1]/screenDims[0];

    // The stepScale needs to be defined (approx space between characters)
    double stepScale = xScale*2;

    // Iterate through all characters to get the the width of the total line of text
    std::string::const_iterator c;
    double lineSize=0;
    if (alignment!=DRAWTEXT::ALIGN_LEFT){
        for(c=text.begin(); c!=text.end(); c++){
            lineSize+=(Characters[*c].Advance >> 6) * stepScale;
        }
    }
    // make the lineSize centre of the line if required
    if(alignment==DRAWTEXT::ALIGN_CENTER){
        lineSize/=2;
    }
    // Create the initial write position of the cursor
    GLfloat x = position[0]-lineSize;
    GLfloat y = position[1]-height*2;

    // Enable the vertexPosition in the GLSL program and bind the vertexbuffer to it
    glEnableVertexAttribArray(vertexPositionID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(
            vertexPositionID,  // The attribute we want to configure
            2,                            // size
            GL_FLOAT,                     // type
            GL_FALSE,                     // normalized?
            0,                            // stride
            (void*)0                      // array buffer offset
    );

    // Enable the UV coordinates in the GLSL program and bind the vertexbuffer to it
    glEnableVertexAttribArray(vertexUVID);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glVertexAttribPointer(
            vertexUVID,                   // The attribute we want to configure
            2,                            // size : U+V => 2
            GL_FLOAT,                     // type
            GL_FALSE,                     // normalized?
            0,                            // stride
            (void*)0                      // array buffer offset
    );
    // Iterate through the line of text and render each glyph on the right position
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];  // the character is used a lot, it's useful to store it in a seperate parameter

        // Alter the transformation matrix to have the correct scale, and postion.
        transformationMatrix[0].x = ch.Size.x * xScale;
        transformationMatrix[0].z = x + (ch.Size.x/2.0 + ch.Bearing.x) * xScale;
        transformationMatrix[1].y = ch.Size.y * yScale;
        transformationMatrix[1].z = y - (ch.Size.y - ch.Bearing.y*2.0) * yScale;

        // Pass the transformation matrix
        glUniformMatrix3fv(tMatrixID,1,GL_FALSE,&transformationMatrix[0][0]);

        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureID);

        // Render quad
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * stepScale; // Bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);   // Unbind the vertex array
    glBindTexture(GL_TEXTURE_2D, 0);    // Unbind the texture
    glUseProgram(0);        // Remove the bound program
}

