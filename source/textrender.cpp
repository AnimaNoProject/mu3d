#include "textrender.h"
QOpenGLShaderProgram *TextRender::_program;
std::map<GLchar, Character> TextRender::Characters;
GLuint TextRender::VAO;
GLuint TextRender::VBO;

void TextRender::initialize()
{

    // get openglfunctions from the current context (important OGLWidget needs to call makeCurrent)
    QOpenGLFunctions_4_5_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();

    TextRender::_program = new QOpenGLShaderProgram();
    TextRender::_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "./shader/text.vert");
    TextRender::_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "./shader/text.frag");
    TextRender::_program->bindAttributeLocation("position", 0);
    TextRender::_program->link();

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

    FT_Face face;
    if (FT_New_Face(ft, "./fonts/arial_narrow_7.ttf", 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

    FT_Set_Pixel_Sizes(face, 0, 48);

    if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;

    f->glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Load first 128 characters of ASCII set
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
            f->glGenTextures(1, &texture);
            f->glBindTexture(GL_TEXTURE_2D, texture);
            f->glTexImage2D(
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
            f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // Now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                uint(face->glyph->advance.x)
            };

            TextRender::Characters.insert(std::pair<GLchar, Character>(c, character));
        }
        f->glBindTexture(GL_TEXTURE_2D, 0);
        // Destroy FreeType once we're finished
        FT_Done_Face(face);
        FT_Done_FreeType(ft);


        f->glGenVertexArrays(1, &TextRender::VAO);
        f->glGenBuffers(1, &TextRender::VBO);
        f->glBindVertexArray(TextRender::VAO);
        f->glBindBuffer(GL_ARRAY_BUFFER, TextRender::VBO);
        f->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        f->glEnableVertexAttribArray(0);
        f->glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
        f->glBindBuffer(GL_ARRAY_BUFFER, 0);
        f->glBindVertexArray(0);
}

void TextRender::RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, QMatrix4x4 projMatrix)
{
    QOpenGLFunctions_4_5_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();

    TextRender::_program->bind();

    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    f->glUniform3f(f->glGetUniformLocation(TextRender::_program->programId(), "textColor"), color.x, color.y, color.z);
    TextRender::_program->setUniformValue(_program->uniformLocation("projection"), projMatrix);
    f->glActiveTexture(GL_TEXTURE0);
    f->glBindVertexArray(TextRender::VAO);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = TextRender::Characters[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }
        };
        // Render glyph texture over quad
        f->glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        f->glBindBuffer(GL_ARRAY_BUFFER, TextRender::VBO);
        f->glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        f->glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        f->glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    f->glBindVertexArray(0);
    f->glBindTexture(GL_TEXTURE_2D, 0);
    f->glDisable(GL_BLEND);
    TextRender::_program->release();
}
