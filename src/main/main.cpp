#include "main/physics.h"
#include "maps/test_1.h"
#include "maps/test_2.h"
#include "maps/outskirts.h"
#include "stb/stb_image.h"
#include "gui/gui.h"

using namespace util;

constinit int frame_rate = 240;
constexpr float WIDTH = 960.0f, HEIGHT = 540.0f;
constinit float scale_x = 0.0f, scale_y = 0.0f; // Only use scale_x, scale_y is to properly scale proportions when the height is changed
constinit float camera_x = 0.0f, camera_y = 0.0f;
auto registry = std::make_shared<Registry>();

template<int height, int width>
void load_map(const int(&)[height][width]);
void map_render();
void entity_render();

void window_close_callback(GLFWwindow* window)
{
    thread::running = false;
}

void frame_buffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    scale_y = height / HEIGHT;
    scale_x = HEIGHT / width * scale_y;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	using namespace key_manager;

    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_W || key == GLFW_KEY_UP) w = true;
        if (key == GLFW_KEY_A || key == GLFW_KEY_LEFT) a = true;
        if (key == GLFW_KEY_S || key == GLFW_KEY_DOWN) s = true;
        if (key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) d = true;
        if (key == GLFW_KEY_SPACE) space = true;
    }
    else if (action == GLFW_RELEASE)
    {
		// Toggle pause
        if (key == GLFW_KEY_ESCAPE) paused = !paused;

		// Movement keys
        if (key == GLFW_KEY_W || key == GLFW_KEY_UP) w = false;
        if (key == GLFW_KEY_A || key == GLFW_KEY_LEFT) a = false;
        if (key == GLFW_KEY_S || key == GLFW_KEY_DOWN) s = false;
        if (key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) d = false;

		// Action key
        if (key == GLFW_KEY_SPACE) space = false;
    }
}

int main(void)
{
	std::println("Starting The Myth of Satonia...");
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WIDTH, HEIGHT, "The Myth of Satonia", nullptr, nullptr);
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
        std::println(stderr, "Failed to load icon image");

    glfwSetWindowCloseCallback(window, window_close_callback);
    glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // Call frame_buffer_size_callback at start to define scaleX and scaleY properly to ensure rendering
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    frame_buffer_size_callback(window, width, height);

    std::println("Window created");
    std::println("Loading projection...");

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1); // Same as default clip space
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	std::println("Done loading projection");
	std::println("Loading textures...");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Texture::load_textures();
	std::println("Loaded {} textures", Texture::s_textures.size());

    try
    {
        load_font("res/fonts/HydrophiliaIced-Regular.ttf");
    }
    catch (const std::exception& e) 
    { 
        std::println(stderr, "Failed to load font \"HydrophiliaIced-Regular.ttf\": {}", e.what()); 
		std::println(stderr, "Try maybe redownloading the font file or using a different one if font is not present and check that the font name matches the font name in the font path");
    }
    std::println("Font loaded");

    std::println("Done loading textures");
    std::println("Creating player...");

    // VSync
    glfwSwapInterval(settings::v_sync);

    // Player
    Entity player = registry->create_entity();
    registry->add_component<Transform>(player).position = { 2.4f, 1.6f };
    registry->add_component<Sprite>(player).texture_id = std::byte(21);
    registry->add_component<Collision>(player).enabled = true;
    registry->add_component<Speed>(player).speed = 0.5f;
    registry->add_component<Health>(player);
    registry->add_component<Range>(player).range = 0.125f;
    registry->add_component<Inventory>(player);
    registry->add_component<Player>(player);

    std::println("Done creating player");
    std::println("Starting physics thread...");

    // Start the physics thread
    start(registry);

    /* Loop until the user closes the window */
    using namespace std::chrono;

    auto target_time = nanoseconds(NANOSECOND / frame_rate);
    auto last_time = high_resolution_clock::now();
    while (!glfwWindowShouldClose(window))
    {
        auto current_time = high_resolution_clock::now();
        float delta_time = duration<float>(current_time - last_time).count();
        last_time = current_time;

        if (settings::fullscreen)
        {
            glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
            glfwMaximizeWindow(window);
        }
        else
            glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);

        glfwGetFramebufferSize(window, &width, &height);
        glfwGetCursorPos(window, &mouse_manager::x, &mouse_manager::y);

        glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ?
            mouse_manager::left = true : mouse_manager::left = false;
        glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS ?
            mouse_manager::right = true : mouse_manager::right = false;

        Transform* transform = registry->get_component<Transform>(player);

        camera_x = transform->position.x + transform->size.x / 2;
        camera_y = transform->position.y + transform->size.y / 2;

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // Handle map loading, the map is set to test map
        if (!map::map_loaded)
        {
            std::println("Loading map ({})...", map::map_to_load);

            Tile::s_tiles.clear();
            test_1::clear(registry);
            test_2::clear(registry);
            outskirts::clear(registry);

            try
            {
                switch (map::map_to_load)
                {
                case 1:
                    load_map(test_1::get_map());
                    std::println("Loading {} entities...", test_1::load_entities(registry));
                    std::println("Done loading entities");
                    break;
                case 2:
                    load_map(test_2::get_map());
                    std::println("Loading {} entities...", test_2::load_entities(registry));
                    std::println("Done loading entities");
                    break;
                case 3:
                    load_map(outskirts::get_map());
                    std::println("Loading {} entities...", outskirts::load_entities(registry));
                    std::println("Done loading entities");
                    break;
                default:
                    std::println(stderr, "Failed to load map, invalid map selected");
                    break;
                }
            } 
            catch (const std::exception& e) 
            { std::println(stderr, "Failed to load map: {}", e.what()); }

            std::println("Done loading map");
            map::map_loaded = true;
        }

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1, 1, -1, 1, -1, 1); // NDC
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        try { map_render(); }
        catch (const std::exception& e)
        { std::println(stderr, "Failed to render map: {}", e.what()); }

        try { entity_render(); }
        catch (const std::exception& e)
        { std::println(stderr, "Failed to render entities: {}", e.what()); }

		// Time of day overlay
        glBegin(GL_QUADS);
        glColor4f(0.0f, 0.0f, 0.0f, time::day);
        glVertex2f(-1.0f, -1.0f);
        glVertex2f(1.0f, -1.0f);
        glVertex2f(1.0f, 1.0f);
        glVertex2f(-1.0f, 1.0f);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glEnd();

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, width, height, 0, -1, 1); // pixel coordinates
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        try { gui_render(registry, player, static_cast<float>(width), static_cast<float>(height)); }
        catch (const std::exception& e)
		{ std::println(stderr, "Failed to render GUI: {}", e.what()); }

        glBegin(GL_QUADS);
        glColor4f(0.0f, 0.0f, 0.0f, 1.0f - settings::brightness);
        glVertex2f(0, 0);
        glVertex2f(static_cast<float>(width), 0);
        glVertex2f(static_cast<float>(width), static_cast<float>(height));
        glVertex2f(0, static_cast<float>(height));
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // Set back to default color to prevent visual bugs
        glEnd();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        float fps = 1.0f / delta_time;
        //std::println("{}", fps);

        if (delta_time < duration<float>(target_time).count())
            std::this_thread::sleep_until(current_time + target_time);
    }

	std::println("Closing...");

    glfwTerminate();
    return 0;
}

void entity_render()
{
    auto sprites = registry->get_all<Sprite>();

    for (auto& pair : sprites)
    {
        Entity entity = pair.first;
        Sprite& sprite = pair.second;
        Transform* transform = registry->get_component<Transform>(entity);
        if (!transform) continue;

        Texture* texture = Texture::s_textures[static_cast<int>(sprite.texture_id)].get();

        float x = transform->position.x - camera_x;
        float y = transform->position.y - camera_y;

        float top = y + transform->size.y;
        float bottom = y;
        float left = x * scale_x;
        float right = (x + transform->size.x) * scale_x;

        if (right < -1 || left > 1) continue;
        if (top < -1 || bottom > 1) continue;

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
	// Reserves space for all tiles to prevent reallocation
	Tile::s_tiles.reserve(width * height);
    constexpr float SIZE = Tile::s_size;
    for (int row = 0; row < height; row++)
        for (int col = 0; col < width; col++)
            if (map[row][col] != 0)
                Tile _{ SIZE * col, SIZE * (height - row - 1), static_cast<std::byte>(map[row][col]) };
}

void map_render()
{
    for (auto pair : Tile::s_tiles)
    {
        Tile& tile = pair.second;

        // id of 0 is the void, so it's ignored
        if (tile.get_id() == std::byte(0)) continue;
        Texture* texture = Texture::s_textures[static_cast<int>(tile.get_id()) - 1].get();

        float x = tile.get_position().x - camera_x;
        float y = tile.get_position().y - camera_y;

        float top = y + Tile::s_size;
        float bottom = y;
        float left = x * scale_x;
        float right = (x + Tile::s_size) * scale_x;

        if (right < -1 || left > 1) continue;
        if (top < -1 || bottom > 1) continue;

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