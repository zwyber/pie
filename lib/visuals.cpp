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
Window::Window(int width, int  height, const unsigned flag){
    winHeight = height;
    winWidth = width;
    winWtHratio = (double)width/height;
    boundUniverse = NULL;
    activeFlag = flag;
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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Couple the pointer to this Window to that of the GLFW window pointer
    // This will be used in WindowResizeStaticCallback (see .h class definition)
    glfwSetWindowUserPointer(GLFWpointer, this);

    // Call th function WindowResizeStaticCallback (which refers to window_size_callback)
    // when the window gets rescaled.
    glfwSetWindowSizeCallback(GLFWpointer, WindowResizeStaticCallback);
    cursorPos = {0,0};
    glfwSetCursorPosCallback(GLFWpointer, cursor_position_callback);

}

void Window::pace_frame() {

    // Check how much time we have left
    double delta = glfwGetTime() - lastTime;
    if (delta < 1/fps) {
        usleep(unsigned((1/fps - delta)*1E6));

        if (delta/(1.0/fps) > 0.75) {
            std::cerr << "WARNING at 75% of CPU time per frame" << std::endl;
        }
    }
    else{
        std::cerr << "CANNOT REACH TARGET FPS" << std::endl;
    }

    lastTime = glfwGetTime();

}

void Window::bindUniverse(Universe *uni) {
    boundUniverse = uni;
    if(boundUniverse!=NULL) {
        if (activeFlag == vis::AUTO_SIZE_UNIVERSE) {
            boundUniverse->resize(winWidth, winHeight);
        }
        uniToWinRatio = {boundUniverse->width * pixRatio / winWidth, boundUniverse->height * pixRatio / winHeight};
        window_size_callback(winWidth, winHeight);
    }
}

void Window::Resize(int width, int height){
    if(width<1){
        width = winWidth;
    }
    if(height<1){
        height = winHeight;
    }
    window_size_callback(width,height);
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
void Window::drawObjectList(CircleShader* circleShader) {
    this->drawObjectList(this->boundUniverse->objects, circleShader);
}

void Window::drawObjectList(std::vector<Object*> &objects, CircleShader* circleShader){
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
            circleShader->tMatrixReset();
            circleShader->tMatrixScale((vec2d){radius/winWtHratio,radius});
            circleShader->tMatrixTranslate(position);
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
    glViewport(0,0, width, height);
}
void Window::cursor_position_callback(double xpos, double ypos){
    cursorPos = {2*xpos/winWidth -1, -2*ypos/winHeight +1};
}
vec2d Window::windowSize() {return {(double)winWidth,(double)winHeight};}
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

vec2d Window::cursorPosition(){
    return cursorPos;
}

/*
 * Shaders section
 */
Shader::Shader(const char *vertexShader, const char *fragmentShader, const char *vertexName, const char *tMatrixName) {
    transformationMatrix = glm::mat3(1.0f);
    programID = LoadShaders(vertexShader, fragmentShader);
    vertexPositionID = glGetAttribLocation(programID, vertexName);

    if(tMatrixName!=NULL){
        tMatrixID = glGetUniformLocation(programID, tMatrixName);
        tMatrixOn = true;
    } else{
        tMatrixOn = false;
    }

    static const GLfloat vertices[] = {
            -1.0f,-1.0f,
            -1.0f,1.0f,
            1.0f,1.0f,
            1.0f,-1.0f
    };

    vertexCount = sizeof(vertices)/(sizeof(GLfloat)*2);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}
Shader::~Shader(){
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteProgram(programID);
}
void Shader::tMatrixReset(){
    transformationMatrix = glm::mat3(1.0f);
}
void Shader::tMatrixRotate(GLfloat angle) {
    const GLfloat c = cos(angle);
    const GLfloat s = sin(angle);
    transformationMatrix *= glm::mat3(
            c, -s, 0,
            s,  c, 0,
            0,  0, 1
    );
}
void Shader::tMatrixScale(vec2d scale) {
    transformationMatrix *= glm::mat3(
            scale[0], 0, 0,
            0, scale[1], 0,
            0,        0, 1
    );
}
void Shader::tMatrixTranslate(vec2d position) {
    transformationMatrix *= glm::mat3(
            1, 0, position[0],
            0, 1, position[1],
            0, 0, 1
    );
}
void Shader::setNewVertices(GLuint arraySize, const GLfloat *vertexArray) {
    vertexCount = arraySize/2;
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*arraySize, vertexArray, GL_STATIC_DRAW);
}
void Shader::draw(){
    glUseProgram(programID);

    if(tMatrixOn) glUniformMatrix3fv(tMatrixID, 1, GL_FALSE, &transformationMatrix[0][0]);

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

    glDrawArrays(GL_TRIANGLE_FAN, 0,vertexCount);

    glDisableVertexAttribArray(vertexPositionID);
    glUseProgram(0);
}

TextureShader::TextureShader(GLuint texture_) : Shader("shaders/texture.glvs", "shaders/texture.glfs", "PositionVec", "MVP"){
    texture = texture_;
    vertexUVID = glGetAttribLocation(programID, "vertexUV");
    textureID  = glGetUniformLocation(programID, "tex");

    static const GLfloat texcoords[] = {
            0.0f,1.0f,
            0.0f,0.0f,
            1.0f,0.0f,
            1.0f,1.0f
    };

    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

}
void TextureShader::setNewUVCoordinates(GLuint arraySize, const GLfloat *uvArray) {
    unsigned uvCount = arraySize/2;
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
    }
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertexCount*2, uvArray, GL_STATIC_DRAW);
}
void TextureShader::setNewUVCoordinates(GLuint arraySize, const GLfloat *uvArray, const GLfloat *vertexArray){
    vertexCount = arraySize/2;

    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*arraySize, uvArray, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*arraySize, vertexArray, GL_STATIC_DRAW);
}
TextureShader::~TextureShader(){
    glDeleteBuffers(1, &uvBuffer);
    glDeleteTextures(1, &textureID);
}
void TextureShader::draw(unsigned texNum){
    glUseProgram(programID);
    glUniformMatrix3fv(tMatrixID, 1, GL_FALSE, &transformationMatrix[0][0]);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glUniform1i(textureID, 0);

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

    glDrawArrays(GL_TRIANGLE_FAN, texNum*4,4);

    glDisableVertexAttribArray(vertexPositionID);
    glDisableVertexAttribArray(vertexUVID);
    glUseProgram(0);
}


CircleShader::CircleShader(glm::vec4 colour_) : Shader("shaders/circle.glvs", "shaders/circle.glfs", "inPosition", "projection"){
    colour = colour_;
    vertexUVID = glGetAttribLocation(programID, "inTexcoord");
    colourID  = glGetUniformLocation(programID, "disc_color");

    static const GLfloat texcoords[] = {
            0.0f,0.0f,
            0.0f,1.0f,
            1.0f,1.0f,
            1.0f,0.0f
    };

    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

}
void CircleShader::setNewUVCoordinates(GLuint arraySize, const GLfloat *uvArray) {
    unsigned uvCount = arraySize/2;
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
    }
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertexCount*2, uvArray, GL_STATIC_DRAW);
}
void CircleShader::setNewUVCoordinates(GLuint arraySize, const GLfloat *uvArray, const GLfloat *vertexArray){
    vertexCount = arraySize/2;

    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*arraySize, uvArray, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*arraySize, vertexArray, GL_STATIC_DRAW);
}
CircleShader::~CircleShader(){
    glDeleteBuffers(1, &uvBuffer);
}
void CircleShader::draw(){
    glUseProgram(programID);
    glUniformMatrix3fv(tMatrixID, 1, GL_FALSE, &transformationMatrix[0][0]);

    glUniform4f(colourID, colour.r, colour.g, colour.b, colour.a);

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

    glDrawArrays(GL_TRIANGLE_FAN, 0,vertexCount);

    glDisableVertexAttribArray(vertexPositionID);
    glDisableVertexAttribArray(vertexUVID);
    glUseProgram(0);
}

TextShader::TextShader(const char* trueTypePath, int numOfChars) : Shader("shaders/text.glvs", "shaders/text.glfs", "VertexPos", "projection"){
    if (FT_Init_FreeType(&ft))
    std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

    if (FT_New_Face(ft, trueTypePath, 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    pixSize = 72;
    FT_Set_Pixel_Sizes(face, 0, pixSize);
    colour = glm::vec4(1.0f);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction


    for (GLubyte c = 0; c < numOfChars; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
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
        // Set texture options
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
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    static const GLfloat texcoords[] = {
            0.0f,1.0f,
            0.0f,0.0f,
            1.0f,0.0f,
            1.0f,1.0f
    };

    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

    vertexUVID = glGetAttribLocation(programID, "vertexUV");
    textureID  = glGetUniformLocation(programID, "text");
    textColorID = glGetUniformLocation(programID, "textColor");

}
TextShader::~TextShader(){
    glDeleteBuffers(1, &uvBuffer);
    for (map<char,Character>::iterator c = Characters.begin(); c != Characters.end(); c++){
        glDeleteTextures(1,&(*c).second.textureID);
    }
}
void TextShader::draw(std::string text, vec2d position, unsigned alignment,vec2d screenDims, double height){
    // Activate corresponding render state
    glUseProgram(programID);
    glUniform3f(textColorID, colour.x, colour.y, colour.z);
    glActiveTexture(GL_TEXTURE0);
    glUniform1f(textureID, 0);
    double yScale = 2 * height / pixSize;
    double xScale = yScale;
    if(screenDims[0] && screenDims[1])
        xScale *= screenDims[1]/screenDims[0];

    double stepScale = xScale*2;
    // Iterate through all characters
    std::string::const_iterator c;
    double lineSize=0;
    if (alignment!=DRAWTEXT::ALIGN_LEFT){
        for(c=text.begin(); c!=text.end(); c++){
            lineSize+=(Characters[*c].Advance >> 6) * stepScale;
        }
    }
    if(alignment==DRAWTEXT::ALIGN_CENTER){
        lineSize/=2;
    }
    GLfloat x = position[0]-lineSize;
    GLfloat y = position[1]-height*2;
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
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        GLfloat xpos = x + (ch.Size.x/2.0 + ch.Bearing.x) * xScale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y*2.0) * yScale;

        GLfloat w = ch.Size.x * xScale;
        GLfloat h = ch.Size.y * yScale;

        transformationMatrix[0].x = w;
        transformationMatrix[0].z = xpos;
        transformationMatrix[1].y = h;
        transformationMatrix[1].z = ypos;

        glUniformMatrix3fv(tMatrixID,1,GL_FALSE,&transformationMatrix[0][0]);

        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureID);

        // Render quad
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * stepScale; // Bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}
