#include "main/texture.h"

#include "stb/stb_image.h"

Texture::Texture(const std::string& path)
	: m_path(path), m_buffer(nullptr), m_width(0), m_height(0), m_nr_channels(0)
{
	stbi_set_flip_vertically_on_load(1);

	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	m_buffer = reinterpret_cast<std::byte*>(stbi_load(path.c_str(), &m_width, &m_height, &m_nr_channels, 4));

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_buffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (m_buffer)
		stbi_image_free(m_buffer);
	else
		std::println("Failed to load texture at \"{}\"", path);

}

void Texture::load_textures() {
	// Pre v0.3 textures
	// Tile
	s_textures[0] = std::make_unique<Texture>("res/textures/tile/floor.png");
	s_textures[1] = std::make_unique<Texture>("res/textures/tile/wall.png");
	s_textures[2] = std::make_unique<Texture>("res/textures/tile/brick.png");
	s_textures[3] = std::make_unique<Texture>("res/textures/tile/water.png");
	s_textures[4] = std::make_unique<Texture>("res/textures/tile/placeholder.png");

	// Entity
	s_textures[5] = std::make_unique<Texture>("res/textures/entity/placeholder.png");
	s_textures[6] = std::make_unique<Texture>("res/textures/entity/chest_closed.png");
	s_textures[7] = std::make_unique<Texture>("res/textures/entity/chest_opened.png");
	s_textures[8] = std::make_unique<Texture>("res/textures/entity/door_locked.png");
	s_textures[9] = std::make_unique<Texture>("res/textures/entity/door_closed.png");
	s_textures[10] = std::make_unique<Texture>("res/textures/entity/door_opened.png");

	// Item
	s_textures[11] = std::make_unique<Texture>("res/textures/item/key.png");

	// Menu
	s_textures[12] = std::make_unique<Texture>("res/textures/menu/logo.png");
	s_textures[13] = std::make_unique<Texture>("res/textures/menu/subscreen_background.png");

	// v0.3 textures
	// Tile
	s_textures[14] = std::make_unique<Texture>("res/textures/tile/grass.png");
	s_textures[15] = std::make_unique<Texture>("res/textures/tile/sand.png");

	// v0.4 textures
	// Tile
	s_textures[16] = std::make_unique<Texture>("res/textures/tile/path.png");
	s_textures[17] = std::make_unique<Texture>("res/textures/tile/path_up.png");
	s_textures[18] = std::make_unique<Texture>("res/textures/tile/path_down.png");
	s_textures[19] = std::make_unique<Texture>("res/textures/tile/path_left.png");
	s_textures[20] = std::make_unique<Texture>("res/textures/tile/path_right.png");

	// Entity
	s_textures[21] = std::make_unique<Texture>("res/textures/entity/player.png");
	s_textures[22] = std::make_unique<Texture>("res/textures/entity/slime.png");
	s_textures[23] = std::make_unique<Texture>("res/textures/entity/gate.png");
	s_textures[24] = std::make_unique<Texture>("res/textures/entity/log.png");
	s_textures[25] = std::make_unique<Texture>("res/textures/entity/stump.png");
	s_textures[26] = std::make_unique<Texture>("res/textures/entity/leaves.png");
	s_textures[27] = std::make_unique<Texture>("res/textures/entity/leaves_up.png");
	s_textures[28] = std::make_unique<Texture>("res/textures/entity/leaves_down.png");
	s_textures[29] = std::make_unique<Texture>("res/textures/entity/leaves_left.png");
	s_textures[30] = std::make_unique<Texture>("res/textures/entity/leaves_right.png");
	s_textures[31] = std::make_unique<Texture>("res/textures/entity/leaves_upper_left.png");
	s_textures[32] = std::make_unique<Texture>("res/textures/entity/leaves_upper_right.png");
	s_textures[33] = std::make_unique<Texture>("res/textures/entity/leaves_lower_left.png");
	s_textures[34] = std::make_unique<Texture>("res/textures/entity/leaves_lower_right.png");
	s_textures[35] = std::make_unique<Texture>("res/textures/entity/blank.png");

	// Menu
	s_textures[36] = std::make_unique<Texture>("res/textures/menu/button_off.png");
	s_textures[37] = std::make_unique<Texture>("res/textures/menu/button_on.png");
}