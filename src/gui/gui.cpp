#include "gui/gui.h"

using namespace util;

constinit stbtt_bakedchar cdata[96]{}; // ASCII 32..126
constinit GLuint ftex = 0;

constexpr float LETTER_WIDTH = 6.0f;

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

void box_render(Box& box, bool& selecting)
{
    glColor4f(0.6f, 0.8f, 1.0f, 1.0f);

    if (mouse_manager::x >= box.get_position().x &&
        mouse_manager::x <= box.get_position().x + box.get_bounds().x &&
        mouse_manager::y >= box.get_position().y &&
        mouse_manager::y <= box.get_position().y + box.get_bounds().y)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        if (mouse_manager::left && !selecting)
            box.set_toggled(!box.is_toggled()),
            selecting = true;
    }

    // Reset the single click toggle when mouse button is released
    if (!mouse_manager::left)
        selecting = false;

    // Draw box toggle
    if (box.is_toggled())
    {
        Texture* texture = Texture::s_textures[37].get();
        texture->bind();
        glBegin(GL_QUADS);

        float left = box.get_position().x;
        float right = box.get_position().x + box.get_bounds().x;
        float bottom = box.get_position().y;
        float top = box.get_position().y + box.get_bounds().y;

        glTexCoord2f(0, 1); glVertex2f(left, top);
        glTexCoord2f(1, 1); glVertex2f(right, top);
        glTexCoord2f(1, 0); glVertex2f(right, bottom);
        glTexCoord2f(0, 0); glVertex2f(left, bottom);

        glEnd();
        texture->unbind();
    }
    else
    {
        Texture* texture = Texture::s_textures[36].get();
        texture->bind();
        glBegin(GL_QUADS);
        float left = box.get_position().x;
        float right = box.get_position().x + box.get_bounds().x;
        float bottom = box.get_position().y;
        float top = box.get_position().y + box.get_bounds().y;

        glTexCoord2f(0, 1); glVertex2f(left, top);
        glTexCoord2f(1, 1); glVertex2f(right, top);
        glTexCoord2f(1, 0); glVertex2f(right, bottom);
        glTexCoord2f(0, 0); glVertex2f(left, bottom);
        glEnd();
        texture->unbind();
    }
}

void button_render(Button& button, Point offset, float scale, float r = 0.6f, float g = 0.8f, float b = 1.0f, float a = 1.0f)
{
    glColor4f(r, g, b, a);

    if (mouse_manager::x >= button.get_position().x &&
        mouse_manager::x <= button.get_position().x + button.get_bounds().x &&
        mouse_manager::y >= button.get_position().y &&
        mouse_manager::y <= button.get_position().y + button.get_bounds().y)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        if (mouse_manager::left)
            button.get_callback()();
    }

    draw_text
    (
        button.get_position().x / scale + offset.x,
        button.get_position().y / scale + offset.y,
        button.get_text().c_str(),
        scale
    );
}

void slider_render(Slider& slider, bool& adjusting)
{
    // Brightness slider
    float x1 = slider.get_position().x;
    float y1 = slider.get_position().y;
    float w1 = slider.get_bounds().x;
    float h1 = slider.get_bounds().y;

    float x2 = slider.get_slider_position().x;
    float y2 = slider.get_slider_position().y;
    float w2 = slider.get_slider_bounds().x;
    float h2 = slider.get_slider_bounds().y;

    // Disable texturing to allow rectangles being drawn
    glDisable(GL_TEXTURE_2D);

    glBegin(GL_QUADS);
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
    glVertex2f(x1, y1);
    glVertex2f(x1, y1 + h1);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glVertex2f(x1 + w1, y1 + h1);
    glVertex2f(x1 + w1, y1);
    glEnd();

    for (auto& pair : slider.get_notch_map())
    {
		Point pos = pair.first;

        glBegin(GL_QUADS);
        glVertex2f(pos.x, pos.y);
        glVertex2f(pos.x + 5, pos.y);
        glVertex2f(pos.x + 5, pos.y + 30);
        glVertex2f(pos.x, pos.y + 30);
        glEnd();
    }

    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

    glBegin(GL_QUADS);
    glVertex2f(x2, y2);
    glVertex2f(x2 + w2, y2);
    glVertex2f(x2 + w2, y2 + h2);
    glVertex2f(x2, y2 + h2);
    glEnd();

    if (mouse_manager::x >= slider.get_position().x &&
        mouse_manager::x <= slider.get_position().x + slider.get_bounds().x &&
        mouse_manager::y >= slider.get_position().y &&
        mouse_manager::y <= slider.get_position().y + slider.get_bounds().y)
    {
        if (mouse_manager::left)
            adjusting = true;
    }

    if (mouse_manager::left == false)
        adjusting = false;

    if (adjusting)
        slider.set_slider_position(static_cast<float>(mouse_manager::x - (slider.get_slider_bounds().x / 2.0f)), slider.get_slider_position().y);

    if (slider.get_slider_position().x < slider.get_position().x)
        slider.set_slider_position(slider.get_position().x, slider.get_slider_position().y);

    if (slider.get_slider_position().x > slider.get_position().x + slider.get_bounds().x)
        slider.set_slider_position(slider.get_position().x + slider.get_bounds().x, slider.get_slider_position().y);

    glEnable(GL_TEXTURE_2D);
}

void gui_render(std::shared_ptr<Registry> registry, Entity player, float width, float height)
{
    hud_render(registry, player);

    if (key_manager::paused) subscreen_render(registry, width, height);

    // Text
    glEnable(GL_TEXTURE_2D);
    draw_text(15, 25, "v0.4.2", 1.5);
    glDisable(GL_TEXTURE_2D);
}

void subscreen_render(std::shared_ptr<Registry> registry, float width, float height)
{
	static constinit bool settings_menu_visible = false;

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
    static Button resume_button
    (
        Point{ width / 2 - 100, height / 2 - 25 },
        Point{ 200, 50 },
        std::string("RESUME"),
        [](){ key_manager::paused = false; }
	);

	resume_button.set_position({ width / 2 - 100, height / 2 - 25 });

    static Button settings_button
    (
        Point{ width / 2 - 100, height / 2 + 50 },
        Point{ 200, 50 },
        std::string("SETTINGS"),
        [](){ settings_menu_visible = true; }
    );

	settings_button.set_position({ width / 2 - 100, height / 2 + 50 });

    static Button quit_button
    (
        Point{ width / 2 - 100, height / 2 + 125 },
        Point{ 200, 50 },
        std::string("QUIT"),
        [](){ glfwSetWindowShouldClose(glfwGetCurrentContext(), true); }
	);

	quit_button.set_position({ width / 2 - 100, height / 2 + 125 });

    if (!settings_menu_visible)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        draw_text(width / 6 - 40, height / 6 - 30, "PAUSED", 3.0f);

		button_render(resume_button, Point{ 25, 25 }, 1.5f);
		button_render(settings_button, Point{ 15, 25 }, 1.5f);
		button_render(quit_button, Point{ 45, 25 }, 1.5f);
    }
    else
        settings_render(registry, width, height, settings_menu_visible);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glDisable(GL_TEXTURE_2D);
	texture->unbind();
}

void settings_render(std::shared_ptr<Registry> registry, float width, float height, bool& visible)
{
    float scale = (540.0f / width) * (height / 540.0f);

    static Button exit
    {
        Point{ width / 2 + 225, height / 2 - 130 },
        Point{ 30, 40 },
        std::string("X"),
        [&](){ visible = false; }
    };

	exit.set_position({ width / 2 + 225, height / 2 - 130 });

    static bool v_sync_box_initialized = false;
    static bool selecting_v_sync = false;
    static Box v_sync
    {
        Point{ width / 2 - 100, height / 2 + 150 },
        Point{ 20, 20 },
    };

    v_sync.set_position({ width / 2 - 100, height / 2 + 150 });

    if (!v_sync_box_initialized)
        v_sync.set_toggled(true), v_sync_box_initialized = true;

    static bool selecting_fullscreen = false;
    static Box fullscreen
    {
        Point{ width / 2 + 100, height / 2 + 150 },
        Point{ 20, 20 },
    };

    fullscreen.set_position({ width / 2 + 100, height / 2 + 150 });

    static bool brightness_slider_initialized = false;
	static bool adjusting_brightness = false;
    static Slider brightness_slider
    {
        Point{ width / 2 - 200, height / 2 + 50 },
        Point{ 400, 20 }
    };

	brightness_slider.set_position({ width / 2 - 200, height / 2 + 50 });

    std::string low_notch_text("LOW");
    std::string medium_notch_text("MED");
    std::string high_notch_text("HIGH");

    if (!brightness_slider_initialized)
    {
        brightness_slider.set_slider_position(brightness_slider.get_slider_position().x + 150, brightness_slider.get_position().y - 10);
        brightness_slider.set_slider_bounds(5, 40);
		brightness_slider.set_minimum(0.4f);
        brightness_slider.set_maximum(1.0f);
        brightness_slider.create_notch(50, brightness_slider.get_position().y - 5, low_notch_text);
        brightness_slider.create_notch(200, brightness_slider.get_position().y - 5, medium_notch_text);
        brightness_slider.create_notch(350, brightness_slider.get_position().y - 5, high_notch_text);
		brightness_slider_initialized = true;
    }

    brightness_slider.set_slider_position(brightness_slider.get_slider_position().x, brightness_slider.get_position().y - 10);
	brightness_slider.set_notch_position(low_notch_text, brightness_slider.get_position().x + 50, brightness_slider.get_position().y - 5);
    brightness_slider.set_notch_position(medium_notch_text, brightness_slider.get_position().x + 200, brightness_slider.get_position().y - 5);
    brightness_slider.set_notch_position(high_notch_text, brightness_slider.get_position().x + 350, brightness_slider.get_position().y - 5);

	slider_render(brightness_slider, adjusting_brightness);
    button_render(exit, Point{ 0, 15 }, 2.5f, 1.0f, 0.0f, 0.0f);
    box_render(v_sync, selecting_v_sync);
	box_render(fullscreen, selecting_fullscreen);

	float x = brightness_slider.get_position().x;
	float w = brightness_slider.get_bounds().x;
    float sx = brightness_slider.get_slider_position().x;
	float min = brightness_slider.get_minimum();
	float max = brightness_slider.get_maximum();

	// Locks the brightness value between min and max based on slider position
    settings::brightness = (max - min) * ((sx - x) / w) + min;
    glfwSwapInterval(settings::v_sync);
    v_sync.is_toggled() ? settings::v_sync = 1 : settings::v_sync = 0;
	fullscreen.is_toggled() ? settings::fullscreen = true : settings::fullscreen = false;

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	draw_text(width / 3 - 100, height / 3 + 80, "V-SYNC", 1.5f);
	draw_text(width / 3 + 10, height / 3 + 80, "FULLSCREEN", 1.5f);
    draw_text(width / 6 - 50, height / 6 - 30, "SETTINGS", 3.0f);
	draw_text(width / 4 - 63, height / 4, "BRIGHTNESS", 2.0f);
}

void hud_render(std::shared_ptr<Registry> registry, Entity player)
{
    int keys = 0;
    auto contents = registry->get_component<Inventory>(player)->contents;
    for (Entity content : contents)
        if (registry->get_component<Key>(content)) keys++;

    std::string key_string = keys == 1 ? "1 Key" : std::to_string(keys) + " Keys";
	std::string health_string = "Health: " + std::to_string((int) registry->get_component<Health>(player)->health);

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
    
    draw_text(50, 50, key_string.c_str(), 1.5);
    
	//draw_text(15, 100, health_string.c_str(), 1.5);

    glDisable(GL_TEXTURE_2D);
    texture->unbind();
}

Point Slider::get_notch_position(const std::string& text)
{
    for (auto it = m_notch.begin(); it != m_notch.end(); ++it)
        if (it->second == text)
            return it->first;

    return { 0.0f, 0.0f };
}

void Slider::set_notch_position(std::string& text, Point pos) 
{
    auto notch = m_notch.find(get_notch_position(text));

    if (notch != m_notch.end())
    {
        m_notch.erase(notch);
        m_notch.insert({ pos, text });
        return;
    }
}
void Slider::set_notch_position(std::string& text, float x, float y) { set_notch_position(text, { x, y }); }