//
// Created by Yvan on 25-8-2016.
// A class based on http://learnopengl.com/#!In-Practice/Text-Rendering

#ifndef PIE_GITHUB_FRAMEWORK_H

#include "framework.h"

#endif

#ifndef TUTORIALS_OGLFONTS_H
#define TUTORIALS_OGLFONTS_H

struct Character{
    GLuint textureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    FT_Pos Advance;
};

struct VertexPos{
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat w;
};


class FontTexHandler{
public:
    GLfloat halfWidth;
    GLfloat halfHeight;
    Shader* shader;
    FontTexHandler(std::string trueTypePath,unsigned pixSize, Shader* shader_, vec2d screenSize);
    void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
private:
    std::map<GLchar, Character> Characters;
    FT_Library ft;
    FT_Face face;
    GLuint VAO, VBO;
    GLuint textColorLocation, vertexPosLocation;
};

#include "OGLfonts.cpp"
#endif //TUTORIALS_OGLFONTS_H
