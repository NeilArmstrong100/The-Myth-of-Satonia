#include <GLFW/glfw3.h>
#include <thread>
#include <chrono>
#include <algorithm>
#include <iostream>
#include "physics.h"
#include "../maps/test.h"
#include "../stb_image/stb_image.h"

bool util::key_manager::w, util::key_manager::a, util::key_manager::s, 
     util::key_manager::d, util::key_manager::space;
int frame_rate = 200;
float scale_x, scale_y; // Only use scale_x, scale_y is to properly scale proportions when the height is changed
float camera_x, camera_y;
bool map_loaded = false;
Player player(2.4f, 1.6f, 0.5f);

template<int height, int width>
void load_map(const int(&)[height][width]);
void map_render();
void entity_render();

void window_close_callback(GLFWwindow* window)
{
    util::thread::running = false;
    util::thread::cv.notify_all();
}

void frame_buffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    scale_y = height / 540.0f;
    scale_x = 540.0f/width * scale_y;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    using namespace util::key_manager;
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_W) w = true;
        if (key == GLFW_KEY_A) a = true;
        if (key == GLFW_KEY_S) s = true;
        if (key == GLFW_KEY_D) d = true;
        if (key == GLFW_KEY_SPACE) space = true;
    }
    else if (action == GLFW_RELEASE)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
            glfwSetWindowShouldClose(window, true);

        if (key == GLFW_KEY_W) w = false;
        if (key == GLFW_KEY_A) a = false;
        if (key == GLFW_KEY_S) s = false;
        if (key == GLFW_KEY_D) d = false;
        if (key == GLFW_KEY_SPACE) space = false;
    }
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(960, 540, "The Myth of Satonia", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Load the icon
    GLFWimage icon_image[1];
    icon_image[0].pixels = stbi_load("res/menu/logo.png", &icon_image[0].width, &icon_image[0].height, 0, 4);
    if (icon_image[0].pixels)
    {
        glfwSetWindowIcon(window, 1, icon_image);
        stbi_image_free(icon_image[0].pixels);
    }
    else
        std::cout << "Failed to load icon image\n";

    glfwSetWindowCloseCallback(window, window_close_callback);
    glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // Call frame_buffer_size_callback at start to define scaleX and scaleY properly to ensure rendering
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    frame_buffer_size_callback(window, width, height);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Texture::load_textures();

    // VSync
    glfwSwapInterval(1);

    start();

    /* Loop until the user closes the window */
    std::unique_lock<std::mutex> lock(util::thread::mtx);
    while (!glfwWindowShouldClose(window))
    {
        using namespace util::thread;

        cv.wait(lock, [] { return physics_done; });

        camera_x = player.get_position().x + Entity::s_size / 2;
        camera_y = player.get_position().y + Entity::s_size / 2;

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        if (!map_loaded) 
        {
            Tile::s_tiles.clear();
            auto& map = get_test_map();
            load_map(map);
            map_loaded = true;
        }
        map_render();

        glColor3f(1.0f, 1.0f, 1.0f);
        entity_render();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        // Move onto next frame
        physics_done = false;
    }

    glfwTerminate();
    return 0;
}

void entity_render()
{
    for (Entity* entity : Entity::s_entities)
    {
        Texture* texture = Texture::s_textures[entity->get_texture_id()].get();

        float x = entity->get_position().x - camera_x;
        float y = entity->get_position().y - camera_y;
        float top = y + Entity::s_size;
        float bottom = y;
        float left = x * scale_x;
        float right = (x + Entity::s_size) * scale_x;

        (*texture).bind();
        glEnable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex2f(left, top);
        glTexCoord2f(1, 1); glVertex2f(right, top);
        glTexCoord2f(1, 0); glVertex2f(right, bottom);
        glTexCoord2f(0, 0); glVertex2f(left, bottom);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        (*texture).unbind();
    }
}

template<int height, int width>
void load_map(const int (&map)[height][width])
{
    float size = Tile::s_size;
    for (int row = 0; row < height; row++)
        for (int col = 0; col < width; col++)
            if (map[row][col] != 0)
                Tile{ size * col, size * (height - row - 1), (byte)map[row][col] };
}

void map_render()
{
    for (auto pair : Tile::s_tiles)
    {
        Tile tile = pair.second;

        if (tile.get_id() == 0) continue;
        Texture* texture = Texture::s_textures[tile.get_id() - 1].get();

        float x = tile.get_position().x - camera_x;
        float y = tile.get_position().y - camera_y;
        float top = y + Tile::s_size;
        float bottom = y;
        float left = x * scale_x;
        float right = (x + Tile::s_size) * scale_x;

        (*texture).bind();
        glEnable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex2f(left, top);
        glTexCoord2f(1, 1); glVertex2f(right, top);
        glTexCoord2f(1, 0); glVertex2f(right, bottom);
        glTexCoord2f(0, 0); glVertex2f(left, bottom);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        (*texture).unbind();
    }
}

Point util::snap_grid(const Point pos, const float size)
{
    float x = floor(pos.x / size) * size;
    float y = floor(pos.y / size) * size;
    return { x, y };
}