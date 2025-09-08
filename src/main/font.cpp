#include "font.h"

stbtt_bakedchar cdata[96]; // ASCII 32..126
GLuint ftex;

void load_font(const char* font_path)
{
    // Read font file into memory
    std::ifstream ifs(font_path, std::ios::binary | std::ios::ate);
    std::streamsize size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    std::vector<unsigned char> buffer(size);
    ifs.read((char*)buffer.data(), size);

    unsigned char* temp_bitmap = new unsigned char[512 * 512];
    stbtt_BakeFontBitmap(buffer.data(), 0, 32.0, temp_bitmap, 512, 512, 32, 96, cdata);

    // Upload texture to OpenGL
    glGenTextures(1, &ftex);
    glBindTexture(GL_TEXTURE_2D, ftex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    delete[] temp_bitmap;
}

void draw_text(float x, float y, const char* text, float scale = 1.0f)
{
    glBindTexture(GL_TEXTURE_2D, ftex);
    glBegin(GL_QUADS);
    while (*text)
    {
        if (*text >= 32 && *text < 128)
        {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(cdata, 512, 512, *text - 32, &x, &y, &q, 1);

            float x0 = q.x0 * scale;
            float y0 = q.y0 * scale;
            float x1 = q.x1 * scale;
            float y1 = q.y1 * scale;

            glTexCoord2f(q.s0, q.t0); glVertex2f(x0, y0);
            glTexCoord2f(q.s1, q.t0); glVertex2f(x1, y0);
            glTexCoord2f(q.s1, q.t1); glVertex2f(x1, y1);
            glTexCoord2f(q.s0, q.t1); glVertex2f(x0, y1);
        }
        ++text;
    }
    glEnd();
}