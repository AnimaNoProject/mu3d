#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <glm/glm.hpp>
#include <map>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_4_5_Core>

struct Character {
    uint     TextureID;  // ID handle of the glyph texture
    glm::ivec2 Size;       // Size of glyph
    glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
    uint     Advance;    // Offset to advance to next glyph
};

class TextRender
{
public:
    static void initialize();
    static void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, QMatrix4x4 projMatrix);
private:
    static QOpenGLShaderProgram *_program;
    static std::map<GLchar, Character> Characters;
    static GLuint VAO;
    static GLuint VBO;
};
