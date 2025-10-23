#include "gui.h"

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

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    stbtt_BakeFontBitmap(buffer.data(), 0, 32.0, temp_bitmap, 512, 512, 32, 96, cdata);

    // Upload texture to OpenGL
    glGenTextures(1, &ftex);
    glBindTexture(GL_TEXTURE_2D, ftex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

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

void gui_render(Registry& registry, Entity player, float width, float height)
{
    hud_render(registry, player);

    if (util::key_manager::paused) subscreen_render(registry, width, height);

    // Text
    glEnable(GL_TEXTURE_2D);
    draw_text(15, 25, "v0.3.0", 1.5);
    glDisable(GL_TEXTURE_2D);
}

void subscreen_render(Registry& registry, float width, float height)
{
	static bool settings_menu_visible = false;

    Texture* texture = Texture::s_textures[13].get();
	texture->bind();
	glEnable(GL_TEXTURE_2D);
	glColor4f(1.0f, 1.0f, 1.0f, 0.6f);
	glBegin(GL_QUADS);

    glTexCoord2f(0, 1); glVertex2f(0, 0);
    glTexCoord2f(1, 1); glVertex2f(width, 0);
    glTexCoord2f(1, 0); glVertex2f(width, height);
    glTexCoord2f(0, 0); glVertex2f(0, height);

    glEnd();

    // Pause menu buttons
    Button resume_button
    (
        Point{ width / 2 - 100, height / 2 - 25 },
        Point{ 200, 50 },
        std::string("RESUME"),
        []() -> const void* { util::key_manager::paused = false; return nullptr; }
	);

    Button settings_button
    (
        Point{ width / 2 - 100, height / 2 + 50 },
        Point{ 200, 50 },
        std::string("SETTINGS"),
        []() -> const void* { settings_menu_visible = true;  return nullptr; }
    );

    Button quit_button
    (
        Point{ width / 2 - 100, height / 2 + 125 },
        Point{ 200, 50 },
        std::string("QUIT"),
        []() -> const void* { glfwSetWindowShouldClose(glfwGetCurrentContext(), true); return nullptr; }
	);

    if (!settings_menu_visible)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        draw_text(width / 6 - 40, height / 6 - 30, "PAUSED", 3.0f);

        // Resume Button
        glColor4f(0.6f, 0.8f, 1.0f, 1.0f);

        if (util::mouse_manager::x >= resume_button.get_position().x &&
            util::mouse_manager::x <= resume_button.get_position().x + resume_button.get_bounds().x &&
            util::mouse_manager::y >= resume_button.get_position().y &&
            util::mouse_manager::y <= resume_button.get_position().y + resume_button.get_bounds().y)
        {
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            if (util::mouse_manager::left)
                resume_button.get_callback()();
        }

        draw_text
        (
            resume_button.get_position().x / 1.5 + 25,
            resume_button.get_position().y / 1.5 + 25,
            resume_button.get_text().c_str(),
            1.5f
        );

        // Settings Button
        glColor4f(0.6f, 0.8f, 1.0f, 1.0f);

        if (util::mouse_manager::x >= settings_button.get_position().x &&
            util::mouse_manager::x <= settings_button.get_position().x + settings_button.get_bounds().x &&
            util::mouse_manager::y >= settings_button.get_position().y &&
            util::mouse_manager::y <= settings_button.get_position().y + settings_button.get_bounds().y)
        {
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            if (util::mouse_manager::left)
                settings_button.get_callback()();
        }

        draw_text
        (
            settings_button.get_position().x / 1.5 + 15,
            settings_button.get_position().y / 1.5 + 25,
            settings_button.get_text().c_str(),
            1.5f
        );

        // Quit Button
        glColor4f(0.6f, 0.8f, 1.0f, 1.0f);

        if (util::mouse_manager::x >= quit_button.get_position().x &&
            util::mouse_manager::x <= quit_button.get_position().x + quit_button.get_bounds().x &&
            util::mouse_manager::y >= quit_button.get_position().y &&
            util::mouse_manager::y <= quit_button.get_position().y + quit_button.get_bounds().y)
        {
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            if (util::mouse_manager::left)
                quit_button.get_callback()();
        }

        draw_text
        (
            quit_button.get_position().x / 1.5 + 45,
            quit_button.get_position().y / 1.5 + 25,
            quit_button.get_text().c_str(),
            1.5f
        );
    }
    else
        settings_render(registry, width, height, settings_menu_visible);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glDisable(GL_TEXTURE_2D);
	texture->unbind();
}

void settings_render(Registry& registry, float width, float height, bool& visible)
{
    // Settings menu implementation goes here
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

    Button exit
    (
        Point{ width / 2 + 200, height / 2 - 130 },
        Point{ 30, 40 },
        std::string("X"),
        nullptr
    );

    if (util::mouse_manager::x >= exit.get_position().x &&
        util::mouse_manager::x <= exit.get_position().x + exit.get_bounds().x &&
        util::mouse_manager::y >= exit.get_position().y &&
        util::mouse_manager::y <= exit.get_position().y + exit.get_bounds().y)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        if (util::mouse_manager::left)
            visible = false;
    }

    draw_text
    (
        exit.get_position().x / 2,
        exit.get_position().y / 2 + 20,
        exit.get_text().c_str(),
        2.0f
    );

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    draw_text(width / 6 - 40, height / 6 - 30, "SETTINGS", 3.0f);
}

void hud_render(Registry& registry, Entity player)
{
    int keys = 0;
    auto& contents = registry.get_component<Inventory>(player)->contents;
    auto& key = registry.get_all<Key>();
    for (auto& pair : key)
        for (Entity content : contents)
            if (pair.first == content) keys++;

    std::string string = keys == 1 ? "1 Key" : std::to_string(keys) + " Keys";

    // Icons
    Texture* texture = Texture::s_textures[11].get();
    texture->bind();
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    float left = 15;
    float right = 65;
    float bottom = 90;
    float top = 40;

    glTexCoord2f(0, 1); glVertex2f(left, top);
    glTexCoord2f(1, 1); glVertex2f(right, top);
    glTexCoord2f(1, 0); glVertex2f(right, bottom);
    glTexCoord2f(0, 0); glVertex2f(left, bottom);
    glEnd();

    draw_text(50, 50, string.c_str(), 1.5);
    glDisable(GL_TEXTURE_2D);
    texture->unbind();
}

Button::Button(Point pos, Point bounds, const std::string& text, const void* (*callback)())
    : m_pos(pos), m_bounds(bounds), m_text(text), m_callback(callback)
{}

Button::Button(float x, float y, Point bounds, const std::string& text, const void* (*callback)())
    : m_pos({x, y}), m_bounds(bounds), m_text(text), m_callback(callback)
{}

Button::Button(Point pos, float width, float height, const std::string& text, const void* (*callback)())
    : m_pos(pos), m_bounds({width, height}), m_text(text), m_callback(callback)
{}

Button::Button(float x, float y, float width, float height, const std::string& text, const void* (*callback)())
    : m_pos({x, y}), m_bounds({width, height}), m_text(text), m_callback(callback)
{}

void Button::set_position(Point pos) { m_pos = pos; }
void Button::set_position(float x, float y) { m_pos = {x, y}; }
void Button::set_bounds(Point bounds) { m_bounds = bounds; }
void Button::set_bounds(float width, float height) { m_bounds = {width, height}; }
void Button::set_text(std::string& text) { m_text = text; }

Button::~Button() = default;