#include <GLFW/glfw3.h>
#include <thread>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <string>
#include "physics.h"
#include "../maps/test.h"
#include "../stb/stb_image.h"
#include "font.h"

bool util::key_manager::w, util::key_manager::a, util::key_manager::s, 
     util::key_manager::d, util::key_manager::space;
int frame_rate = 240;
float scale_x, scale_y; // Only use scale_x, scale_y is to properly scale proportions when the height is changed
float camera_x, camera_y;
bool map_loaded = false;
Registry registry;

template<int height, int width>
void load_map(const int(&)[height][width]);
void map_render();
void entity_render();
void gui_render(Registry&, Entity);

void window_close_callback(GLFWwindow* window)
{
    util::thread::running = false;
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
    icon_image[0].pixels = stbi_load("res/textures/menu/logo.png", &icon_image[0].width, &icon_image[0].height, 0, 4);
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

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1); // same as default clip space
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Texture::load_textures();
    load_font("res/fonts/HydrophiliaIced-Regular.ttf");

    // VSync
    glfwSwapInterval(0);

    // Player
    Entity player = registry.create_entity();
    registry.add_component<Transform>(player).position = { 2.4f, 1.6f };
    registry.add_component<Sprite>(player);
    registry.add_component<Collision>(player).enabled = true;
    registry.add_component<Speed>(player).speed = 0.5f;
    registry.add_component<Range>(player).range = 0.125f;
    registry.add_component<Inventory>(player);
    registry.add_component<Player>(player);

    // Enemy
    Entity enemy = registry.create_entity();
    registry.add_component<Transform>(enemy).position = { 1.4f, 1.6f };
    registry.add_component<Sprite>(enemy);
    registry.add_component<Collision>(enemy).enabled = true;
    registry.add_component<Speed>(enemy).speed = 0.3f;
    registry.add_component<Enemy>(enemy);

    // Key
    Entity key = registry.create_entity();
    registry.add_component<Sprite>(key).texture_id = 11;
    registry.add_component<Item>(key);
    registry.add_component<Key>(key);

    Entity power = registry.create_entity();
    registry.add_component<Item>(power);
    registry.add_component<Boots>(power);

    // Chest 1
    Entity chest_1 = registry.create_entity();
    registry.add_component<Transform>(chest_1).position = { 0.4f, 0.6f };
    registry.add_component<Sprite>(chest_1).texture_id = 6;
    registry.add_component<Collision>(chest_1).enabled = true;
    registry.add_component<Useable>(chest_1);
    registry.add_component<Inventory>(chest_1).contents = { key };
    registry.add_component<Chest>(chest_1);

    // Chest 2
    Entity chest_2 = registry.create_entity();
    registry.add_component<Transform>(chest_2).position = { 4.8f, 0.2f };
    registry.add_component<Sprite>(chest_2).texture_id = 6;
    registry.add_component<Collision>(chest_2).enabled = true;
    registry.add_component<Useable>(chest_2);
    registry.add_component<Inventory>(chest_2).contents = { key };
    registry.add_component<Chest>(chest_2);

    // Chest 3
    Entity chest_3 = registry.create_entity();
    registry.add_component<Transform>(chest_3).position = { 4.0f, 0.2f };
    registry.add_component<Sprite>(chest_3).texture_id = 6;
    registry.add_component<Collision>(chest_3).enabled = true;
    registry.add_component<Useable>(chest_3);
    registry.add_component<Inventory>(chest_3).contents = { power };
    registry.add_component<Chest>(chest_3);

    // Door 1
    Entity door_1 = registry.create_entity();
    registry.add_component<Transform>(door_1).position = { 2.4f, 1.2f };
    registry.add_component<Sprite>(door_1).texture_id = 9;
    registry.add_component<Collision>(door_1).enabled = true;
    registry.add_component<Useable>(door_1);
    registry.add_component<Lock>(door_1).open = true;
    registry.add_component<Door>(door_1);

    // Door 2
    Entity door_2 = registry.create_entity();
    registry.add_component<Transform>(door_2).position = { 3.8f, 0.4f };
    registry.add_component<Sprite>(door_2).texture_id = 9;
    registry.add_component<Collision>(door_2).enabled = true;
    registry.add_component<Useable>(door_2);
    registry.add_component<Lock>(door_2).open = false;
    registry.add_component<Door>(door_2);

    // Door 3
    Entity door_3 = registry.create_entity();
    registry.add_component<Transform>(door_3).position = { 2.4f, 0.0f };
    registry.add_component<Sprite>(door_3).texture_id = 9;
    registry.add_component<Collision>(door_3).enabled = true;
    registry.add_component<Useable>(door_3);
    registry.add_component<Lock>(door_3).open = false;
    registry.add_component<Door>(door_3);

    // Door 4
    Entity door_4 = registry.create_entity();
    registry.add_component<Transform>(door_4).position = { 1.4f, 1.2f };
    registry.add_component<Sprite>(door_4).texture_id = 9;
    registry.add_component<Collision>(door_4).enabled = true;
    registry.add_component<Useable>(door_4);
    registry.add_component<Lock>(door_4).open = true;
    registry.add_component<Door>(door_4);

    // Door 5
    Entity door_5 = registry.create_entity();
    registry.add_component<Transform>(door_5).position = { 0.8f, 0.6f };
    registry.add_component<Sprite>(door_5).texture_id = 9;
    registry.add_component<Collision>(door_5).enabled = true;
    registry.add_component<Useable>(door_5);
    registry.add_component<Lock>(door_5).open = true;
    registry.add_component<Door>(door_5);

    // Start the physics thread
    start(registry);

    auto last_frame_time = std::chrono::high_resolution_clock::now();

    /* Loop until the user closes the window */
    auto target_time = std::chrono::nanoseconds(1'000'000'000 / frame_rate);
    auto last_time = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window))
    {
        auto start_time = std::chrono::high_resolution_clock::now();
        float delta_time = std::chrono::duration<float>(start_time - last_time).count();
        last_time = start_time;

        Transform* transform = registry.get_component<Transform>(player);

        camera_x = transform->position.x + transform->size.x / 2;
        camera_y = transform->position.y + transform->size.y / 2;

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // Handle map loading, the map is set to test map
        if (!map_loaded) 
        {
            Tile::s_tiles.clear();
            auto& map = get_test_map();
            load_map(map);
            map_loaded = true;
        }

        glColor3f(1.0f, 1.0f, 1.0f);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1, 1, -1, 1, -1, 1); // NDC
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        map_render();
        entity_render();

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, width, height, 0, -1, 1); // pixel coordinates
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gui_render(registry, player);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        auto current_frame_time = std::chrono::high_resolution_clock::now();

        auto frame_time = current_frame_time - last_frame_time;
        float fps = 1.0f / std::chrono::duration<float>(frame_time).count();
        //std::cout << fps << '\n';
        last_frame_time = current_frame_time;

        auto end_time = std::chrono::high_resolution_clock::now();
        auto work_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);

        if (work_time < target_time)
            std::this_thread::sleep_until(start_time + target_time);
    }

    glfwTerminate();
    return 0;
}

void entity_render()
{
    auto& sprites = registry.get_all<Sprite>();

    for (auto& pair : sprites)
    {
        Entity entity = pair.first;
        Sprite sprite = pair.second;
        Transform* transform = registry.get_component<Transform>(entity);
        if (!transform) continue;

        Texture* texture = Texture::s_textures[sprite.texture_id].get();

        float x = transform->position.x - camera_x;
        float y = transform->position.y - camera_y;
        float top = y + transform->size.y;
        float bottom = y;
        float left = x * scale_x;
        float right = (x + transform->size.x) * scale_x;

        texture->bind();
        glEnable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex2f(left, top);
        glTexCoord2f(1, 1); glVertex2f(right, top);
        glTexCoord2f(1, 0); glVertex2f(right, bottom);
        glTexCoord2f(0, 0); glVertex2f(left, bottom);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        texture->unbind();
    }
}

template<int height, int width>
void load_map(const int (&map)[height][width])
{
    // Puts a new tile into s_tiles with an x, y, and id adjacent to the loaded map
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

        // id of 0 is the void, so it's ignored
        if (tile.get_id() == 0) continue;
        Texture* texture = Texture::s_textures[tile.get_id() - 1].get();

        float x = tile.get_position().x - camera_x;
        float y = tile.get_position().y - camera_y;
        float top = y + Tile::s_size;
        float bottom = y;
        float left = x * scale_x;
        float right = (x + Tile::s_size) * scale_x;

        texture->bind();
        glEnable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex2f(left, top);
        glTexCoord2f(1, 1); glVertex2f(right, top);
        glTexCoord2f(1, 0); glVertex2f(right, bottom);
        glTexCoord2f(0, 0); glVertex2f(left, bottom);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        texture->unbind();
    }
}

void gui_render(Registry& registry, Entity player)
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
    glDisable(GL_TEXTURE_2D);
    texture->unbind();

    // Text
    glEnable(GL_TEXTURE_2D);
    draw_text(15, 25, "v0.2.0", 1.5);
    draw_text(50, 50, string.c_str(), 1.5);
    glDisable(GL_TEXTURE_2D);
}

Point util::snap_grid(const Point pos, const float size)
{
    float x = floor(pos.x / size) * size;
    float y = floor(pos.y / size) * size;
    return { x, y };
}