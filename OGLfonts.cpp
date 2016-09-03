//
// Created by Yvan on 25-8-2016.
//
// A class based on http://learnopengl.com/#!In-Practice/Text-Rendering

#include "OGLfonts.h"

FontTexHandler::FontTexHandler(std::string trueTypePath, unsigned pixSize, GLuint shader_, vec2d screenSize){
    halfWidth = screenSize[0]/2.0f;
    halfHeight = screenSize[1]/2.0f;
    shader = shader_;
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

    if (FT_New_Face(ft, trueTypePath.c_str(), 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

    FT_Set_Pixel_Sizes(face, 0, pixSize);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

    for (GLubyte c = 0; c < 128; c++)
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

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    vertexPosLocation = glGetAttribLocation(shader, "VertexPos" );
    textColorLocation = glGetUniformLocation(shader, "textColor");
    textLocation = glGetUniformLocation(shader, "text");
    projectionLocation = glGetUniformLocation(shader, "projection");

    windowProjection = glm::mat4(1.0f);
}

void FontTexHandler::renderText(std::string text, GLfloat X, GLfloat Y, GLfloat scale, glm::vec3 color)
{
    GLfloat x = X;
    GLfloat y = Y;
    // Activate corresponding render state
    glUseProgram(shader);
    glUniform3f(textColorLocation, color.x, color.y, color.z);
    glUniformMatrix4fv(projectionLocation,1,GL_FALSE,&windowProjection[0][0]);
    glActiveTexture(GL_TEXTURE0);
    glUniform1f(textLocation, 0);
    glBindVertexArray(VAO);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        GLfloat xpos = (x + ch.Bearing.x * scale)/halfWidth;
        GLfloat ypos = (y - (ch.Size.y - ch.Bearing.y) * scale)/halfHeight;

        GLfloat w = ch.Size.x * scale / halfWidth;
        GLfloat h = ch.Size.y * scale / halfHeight;
        // Update VBO for each character
        VertexPos vertices[6] = {
                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos,     ypos,       0.0, 1.0 },
                { xpos + w, ypos,       1.0, 1.0 },

                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos + w, ypos,       1.0, 1.0 },
                { xpos + w, ypos + h,   1.0, 0.0 }
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        glEnableVertexAttribArray( vertexPosLocation );
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glVertexAttribPointer(vertexPosLocation, 4, GL_FLOAT, GL_FALSE, 0,(void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray( 0 );
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}
