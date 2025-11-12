#include <chrono>
#include "physics.h"
#include "../maps/test_1.h"
#include "../maps/test_2.h"
#include "../stb/stb_image.h"
#include "../gui/gui.h"

bool util::key_manager::w, util::key_manager::a, util::key_manager::s, 
     util::key_manager::d, util::key_manager::space, util::key_manager::paused,
     util::mouse_manager::left, util::mouse_manager::right;
double util::mouse_manager::x, util::mouse_manager::y;
int frame_rate = 240, util::map::map_to_load = 1;
float scale_x, scale_y; // Only use scale_x, scale_y is to properly scale proportions when the height is changed
float camera_x, camera_y;
bool util::map::map_loaded = false;
Registry registry;

template<int height, int width>
void load_map(const int(&)[height][width]);
void map_render();
void entity_render();

void window_close_callback(GLFWwindow* window)
{
    util::thread::running = false;
}

void frame_buffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    scale_y = height / 540.0f;
    scale_x = 540.0f / width * scale_y;
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
		// Toggle pause
        if (key == GLFW_KEY_ESCAPE) paused = !paused;

		// Movement keys
        if (key == GLFW_KEY_W) w = false;
        if (key == GLFW_KEY_A) a = false;
        if (key == GLFW_KEY_S) s = false;
        if (key == GLFW_KEY_D) d = false;

		// Action key
        if (key == GLFW_KEY_SPACE) space = false;
    }
}

int main(void)
{
	std::cout << "Starting The Myth of Satonia...\n";
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
        std::cerr << "Failed to load icon image\n";

    glfwSetWindowCloseCallback(window, window_close_callback);
    glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // Call frame_buffer_size_callback at start to define scaleX and scaleY properly to ensure rendering
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    frame_buffer_size_callback(window, width, height);

    std::cout << "Window created\n";
    std::cout << "Loading projection...\n";

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1); // same as default clip space
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	std::cout << "Done loading projection\n";
	std::cout << "Loading textures...\n";

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Texture::load_textures();
	std::cout << "Loaded " << Texture::s_textures.size() << " textures\n";

    try
    {
        load_font("res/fonts/HydrophiliaIced-Regular.ttf");
    }
    catch (const std::exception& e) 
    { 
        std::cerr << "Failed to load font \"HydrophiliaIced-Regular.ttf\": " << e.what() << '\n'; 
		std::cerr << "Try maybe redownloading the font file or using a different one if font is not present and check that the font name matches the font name in the font path\n";
    }
	std::cout << "Font loaded\n";

	std::cout << "Done loading textures\n";
	std::cout << "Creating player...\n";

    // VSync
    glfwSwapInterval(1);

    // Player
    Entity player = registry.create_entity();
    registry.add_component<Transform>(player).position = { 2.4f, 1.6f };
    registry.add_component<Sprite>(player);
    registry.add_component<Collision>(player).enabled = true;
    registry.add_component<Speed>(player).speed = 0.5f;
    registry.add_component<Health>(player);
    registry.add_component<Range>(player).range = 0.125f;
    registry.add_component<Inventory>(player);
    registry.add_component<Player>(player);

	std::cout << "Done creating player\n";
	std::cout << "Starting physics thread...\n";

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

        glfwGetFramebufferSize(window, &width, &height);
		glfwGetCursorPos(window, &util::mouse_manager::x, &util::mouse_manager::y);

        glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ?
            util::mouse_manager::left = true : util::mouse_manager::left = false;
        glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS ?
            util::mouse_manager::right = true : util::mouse_manager::right = false;

        Transform* transform = registry.get_component<Transform>(player);

        camera_x = transform->position.x + transform->size.x / 2;
        camera_y = transform->position.y + transform->size.y / 2;

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // Handle map loading, the map is set to test map
        if (!util::map::map_loaded) 
        {
			std::cout << "Loading map...\n";

            Tile::s_tiles.clear();
            test_1::clear(registry);
            test_2::clear(registry);

            switch (util::map::map_to_load)
            {
            case 1:
                load_map(test_1::get_map());;
                std::cout << "Loading entities...\n";
				test_1::load_entities(registry);
                std::cout << "Done loading entities\n";
                break;
			case 2:
				load_map(test_2::get_map());
                std::cout << "Loading entities...\n";
				test_2::load_entities(registry);
                std::cout << "Done loading entities\n";
                break;
			default:
				std::cerr << "Failed to load map, invalid map selected\n";
                break;
            }

			std::cout << "Done loading map\n";
            util::map::map_loaded = true;
        }

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1, 1, -1, 1, -1, 1); // NDC
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        map_render();
        entity_render();

		// Time of day overlay
        glBegin(GL_QUADS);
        glColor4f(0.0f, 0.0f, 0.0f, util::time::day);
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
        glBegin(GL_QUADS);
        glColor4f(0.0f, 0.0f, 0.0f, util::settings::brightness);
        glVertex2f(0, 0);
        glVertex2f(width, 0);
        glVertex2f(width, height);
        glVertex2f(0, height);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // Set back to default color to prevent visual bugs
        glEnd();
        gui_render(registry, player, width, height);

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

	std::cout << "Closing...\n";

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

Point util::snap_grid(const Point pos, const float size)
{
    float x = floor(pos.x / size) * size;
    float y = floor(pos.y / size) * size;
    return { x, y };
}