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
namespace DRAWTEXT{
    const unsigned ALIGN_LEFT = 0;
    const unsigned ALIGN_CENTER = 1;
    const unsigned ALIGN_RIGHT = 2;
}

class Shader{
protected:
    GLuint tMatrixID;
    GLuint vertexPositionID;
    GLuint vertexBuffer;
    bool tMatrixOn;
    unsigned vertexCount;
public:
    Shader(const char* vertexShader, const char* fragmentShader, const char* vertexName, const char* tMatrixName = NULL);
    ~Shader();
    glm::mat3 transformationMatrix;
    void tMatrixReset();
    void tMatrixRotate(GLfloat angle);
    void tMatrixScale(vec2d scale);
    void tMatrixTranslate(vec2d position);
    void setNewVertices(GLuint arraySize, const GLfloat * vertexArray);
    GLuint programID;
    virtual void draw();
};
class TextureShader: public Shader{
private:
    GLuint vertexUVID;
    GLuint textureID;
    GLuint uvBuffer;
    GLuint texture;
public:
    TextureShader(GLuint texture_);// : Shader("shaders/texture.glvs", "shaders/texture.glfs", "PositionVec", "MVP");
    ~TextureShader();
    void setNewUVCoordinates(GLuint arraySize, const GLfloat * uvArray);
    void setNewUVCoordinates(GLuint arraySize, const GLfloat *uvArray, const GLfloat *vertexArray);
    void draw(unsigned texNum = 0);
};
class CircleShader: public Shader{
private:
    GLuint vertexUVID;
    GLuint colourID;
    GLuint uvBuffer;
public:
    CircleShader(glm::vec4 colour_ = glm::vec4(1.0));// : Shader("shaders/circle.glvs", "shaders/circle.glfs", "inPosition", "projection");
    ~CircleShader();
    glm::vec4 colour;
    void setNewUVCoordinates(GLuint arraySize, const GLfloat * uvArray);
    void setNewUVCoordinates(GLuint arraySize, const GLfloat *uvArray, const GLfloat *vertexArray);
    void draw();
};
//TextShader is a class inspired by: http://learnopengl.com/#!In-Practice/Text-Rendering
class TextShader: public Shader{
private:
    std::map<GLchar, Character> Characters;
    FT_Library ft;
    FT_Face face;
    GLuint textColorID;
    GLuint uvBuffer;
    GLuint vertexUVID;
    GLuint textureID;
    int pixSize;
public:
    glm::vec4 colour;
    TextShader(const char* trueTypePath, int numOfChars = 128);// : Shader("shaders/text.glvs", "shaders/text.glfs", "VertexPos", "projection");
    void draw(std::string text, vec2d position, unsigned alignment = DRAWTEXT::ALIGN_LEFT,vec2d screenDims = {0,0}, double height = 0.1);
};

class Window{
private:
    // Keep the following parameters protected but shared in the draw functions
    int winHeight;
    int winWidth;
    double winWtHratio;
    vec2d cursorPos;
    //// properties in order to resolve universe and window conversions
    // Currently only a single universe can be bound to a window, if needed this can be easily changed to a vector

    unsigned activeFlag;

    //
    vec2d uniToWinRatio;
    void stdInitWindow();
public:

    Universe* boundUniverse;

    // Constructor that makes a window use ready.
    Window(int width = 640, int  height = 480, const unsigned flag = vis::AUTO_SIZE_UNIVERSE);
    Window(Universe* uni, double pixRatio);
    Window(int width, int height, Universe* uni, double pixelRatio, const unsigned flag);

    double fps = 60;
    double lastTime = glfwGetTime();

    void bindUniverse(Universe* uni);
    void Resize(int width =0, int height =0);
    void changeResizeFlag(unsigned flag);
    // The pointer to the actual GLFWwindow this class controls
    GLFWwindow* GLFWpointer;

    // The function that gets called (eventually) when we rescale the window
    void window_size_callback(int width, int height);
    void cursor_position_callback(double xpos, double ypos);
    // Variable that is the number of pixels per physics length unit.
    double pixRatio = 25.0;
// Draws a grid in the current active window, the stepsize is the px size of the grid
    void drawGrid(int stepSize);

// Draws circles. Needs a position for the centre, a radius (normalized to screen height), number of points used to
// define the circle, screenratio (so you actually get circles and not ellipses) and finally it needs a colour
// consisting of {Red, Green, Blue, Alpha}.
    void drawFilledCircle(vec2d &pos, GLdouble &r, int num_segments, std::array<double, 4> Colour);

// Goes through the object list and draws all objects to the current active window
    void drawObjectList(std::vector<Object *> &objects, CircleShader* circleShader = NULL);
    void drawObjectList(CircleShader* circleShader = NULL);

    // Draws a simple red box in the middle of the screen.
    void drawBox(double Width, double Height);

    void drawFT_Bitmap(FT_Bitmap* image, int xleft, int ytop);

    // For frame pacing
    void pace_frame();

    // Convert from px to universe length scale
    vec2d px_to_length (vec2d px);
    vec2d length_to_px (vec2d length);

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
        Window *window = static_cast<Window*>(glfwGetWindowUserPointer(win));
        window->cursor_position_callback(xpos,ypos);
    }
};


#include "visuals.cpp"

#endif //TUTORIALS_VISUALS_H
