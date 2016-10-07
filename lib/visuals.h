//
// Created by Yvan on 10-8-2016.
//

#ifndef PIE_GITHUB_FRAMEWORK_H

#include "framework.h"

#endif

#ifndef TUTORIALS_VISUALS_H
#define TUTORIALS_VISUALS_H

// Constants defining the resize methods of the universe/window
namespace vis{
    const unsigned FIXED_SIZE_UNIVERSE = 0; // Universe does not scale when window rescales
    const unsigned AUTO_SIZE_UNIVERSE = 1;  // Scale universe with window based on pixRatio
    const unsigned PROP_SIZE_UNIVERSE = 2;  // Scale the universe with the
    const unsigned NO_RESIZE = 3;           // No resize of window is possible (NOTE CAN ONLY BE SET AT INITIALISATION)
    const unsigned ZOOM_UNIVERSE = 4;       // When windows resizes the universe pixRatio scales to fit.
}

// constants defining the alignment methods of the textshader
namespace DRAWTEXT{
    const unsigned ALIGN_LEFT = 0;
    const unsigned ALIGN_CENTER = 1;
    const unsigned ALIGN_RIGHT = 2;
}

// Base shader class for easy construction and drawing of shader based graphics.
class Shader{
protected:
    GLuint tMatrixID;       // Location of the transformMatrix in the GLSL program.
    GLuint vertexPositionID;// Location of the vertex input in the GLSL program
    GLuint vertexBuffer;    // Buffer location in memory for vertices
    bool tMatrixOn;         // Determine whether transfomrationMatrix is used
    unsigned vertexCount;   // Amount of vertices to store in vertexBuffer
public:
    Shader(const char* vertexShader, const char* fragmentShader, const char* vertexName, const char* tMatrixName = NULL);
    ~Shader();
    glm::mat3 transformationMatrix;             // a transformationMatrix to control positioning, scaling etc.
    void tMatrixReset();                        // set the transformationMatrix back to the identity matrix
    void tMatrixRotate(GLfloat angle);          // Multiply the tMatrix to rotate 'angle' radians (on the M2x2 part)
    void tMatrixScale(vec2d scale);             // Multiply the tMatrix to scale (on the M2x2 part)
    void tMatrixTranslate(vec2d position);      // Multiply the tMatrix by a translation matrix;
    void setNewVertices(GLuint arraySize, const GLfloat * vertexArray);    // Change the vertices stored in the vertexBuffer
    GLuint programID;    // ID of the GLSL program
    virtual void draw();    // Draw the defined vertices with the GLSL program
};

// A shader class that handles textures
class TextureShader: public Shader{
private:
    GLuint vertexUVID;  // Location of UV coordinates in the GLSL program
    GLuint textureID;   // Location of the texture sampler in the GLSL program
    GLuint uvBuffer;    // Buffer location for UV coordinates
    GLuint texture;     // Buffer location for a texture
public:
    TextureShader(GLuint texture_);// : Shader("shaders/texture.glvs", "shaders/texture.glfs", "PositionVec", "MVP");
    ~TextureShader();
    void setNewUVCoordinates(GLuint arraySize, const GLfloat * uvArray);                            // change the UV coordinates stored in the UVbuffer
    void setNewUVCoordinates(GLuint arraySize, const GLfloat *uvArray, const GLfloat *vertexArray); // change the UV coordinates and the vertex array stored in the Buffers (overload)
    void draw(unsigned texNum = 0);     // Draw a set of UV coordinates named by texNum
};

// A shader class to draw circles with a minimalistic lighting effect
class CircleShader: public Shader{
private:
    GLuint vertexUVID;  // Location of UV coordinates in the GLSL program
    GLuint colourID;    // Location of the circle colour in the GLSL program
    GLuint uvBuffer;    // Buffer location for UV coordinates
public:
    CircleShader(glm::vec4 colour_ = glm::vec4(1.0));// : Shader("shaders/circle.glvs", "shaders/circle.glfs", "inPosition", "projection");
    ~CircleShader();
    glm::vec4 colour;   // vector containing the colour we wish to pass to the program
    void setNewUVCoordinates(GLuint arraySize, const GLfloat * uvArray);                            // change the UV coordinates stored in the UVbuffer
    void setNewUVCoordinates(GLuint arraySize, const GLfloat *uvArray, const GLfloat *vertexArray); // change the UV coordinates and the vertex array stored in the Buffers (overload)
    void draw();
};

//TextShader is a class inspired by: http://learnopengl.com/#!In-Practice/Text-Rendering
class TextShader: public Shader{
private:
    std::map<GLchar, Character> Characters; // A map between Chars and Characters containing textures and scaling
    FT_Library ft;  // FreeType Library
    FT_Face face;   // FreeType face to draw a character into a bitmap
    GLuint textColorID; // Location of text colour in the GLSL program
    GLuint uvBuffer;    // Buffer location for UV coordinates
    GLuint vertexUVID;  // Location of UV coordinates in the GLSL program
    GLuint textureID;   // Location of the texture sampler in the GLSL program
    int pixSize;        // pixel size of the characters (pts)
public:
    TextShader(const char* trueTypePath, int numOfChars = 128);// : Shader("shaders/text.glvs", "shaders/text.glfs", "VertexPos", "projection");
    ~TextShader();
    glm::vec4 colour;   // vector containing the colour we wish to pass to the program
    // draw command to render text in a line with in FreeType generated spacing.
    void draw(std::string text, vec2d position, unsigned alignment = DRAWTEXT::ALIGN_LEFT,vec2d screenDims = {0,0}, double height = 0.1);
};

class Window{
private:
    // Keep the following parameters protected but shared in the draw functions
    // Window size properties
    int winHeight;
    int winWidth;
    double winWtHratio;

    // Accesible storage for the cursor position (based on cursor callback)
    vec2d cursorPos;

    // Flag (see vis namespace) that defines the
    unsigned activeFlag;

    // Saved universe to window ratio for rescaling
    vec2d uniToWinRatio;

    // Basic initiation function called by all window constructors
    void stdInitWindow();

public:
    // The universe that is bound to the window (the window can control its size)
    Universe* boundUniverse;

    // The pointer to the actual GLFWwindow this class controls
    GLFWwindow* GLFWpointer;
    // Variable that is the number of pixels per physics length unit.
    double pixRatio = 25.0;

    //// Constructor that makes a window use ready.
    // Generate a window of size width height, without a bound universe (flag is for resize)
    Window(int width = 640, int  height = 480, const unsigned flag = vis::AUTO_SIZE_UNIVERSE);
    // Generate a window to fit universe with auto resize of the boundUniverse when the window is resized
    Window(Universe* uni, double pixRatio);
    // Generate a window of size width x height, with a univers bound to it (pixelratio for pixels per univers lenghtunit and flag for resize)
    Window(int width, int height, Universe* uni, double pixelRatio, const unsigned flag);

    //// Time parameters
    // set a standard refresh rate for the paceframe function.
    double fps = 60;
    // define a variable that stores the time.
    double lastTime = glfwGetTime();

    // Funtion needed to process a rebind of the universe correctly.
    void bindUniverse(Universe* uni);
    // Resize window / universe in the appropriate way (defined by activeFlag and for external use)
    void Resize(int width =0, int height =0);
    // Change the resize method
    void changeResizeFlag(unsigned flag);

    // The function that gets called (eventually) when we rescale the window
    void window_size_callback(int width, int height);
    // The function for the cursor that eventually gets called by the static callback function cursor_position_callback
    void cursor_position_callback(double xpos, double ypos);

    // Draws a grid in the current active window, the stepsize is the px size of the grid
    void drawGrid(int stepSize);

    // Draws circles. Needs a position for the centre, a radius (normalized to screen height), number of points used to
    // define the circle, screenratio (so you actually get circles and not ellipses) and finally it needs a colour
    // consisting of {Red, Green, Blue, Alpha}.
    void drawFilledCircle(vec2d &pos, GLdouble &r, int num_segments, std::array<double, 4> Colour);

    // Goes through the object list and draws all objects to the current active window
    void drawObjectList(std::vector<Object *> &objects, CircleShader* circleShader = NULL);
    // Use the bound universe
    void drawObjectList(CircleShader* circleShader = NULL);

    // Draws a simple red box in the middle of the screen.
    void drawBox(double Width, double Height);

    // For frame pacing
    void pace_frame();

    //Get window size or cursor position
    vec2d windowSize();
    vec2d cursorPosition();
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
    // Similar method is used for the cursor position callback the even though the public internal function 'cur'
    // only sets the position having this separated prevents accidental writes.
    static void cursor_position_callback(GLFWwindow* win, double xpos, double ypos)
    {
        // Get the window pointer of this window from glfw
        Window *window = static_cast<Window*>(glfwGetWindowUserPointer(win));
        // Call the actual cursor position.
        window->cursor_position_callback(xpos,ypos);
    }
};


#include "visuals.cpp"

#endif //TUTORIALS_VISUALS_H
