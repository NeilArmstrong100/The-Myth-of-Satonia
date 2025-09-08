#include "texture.h"

#include "../stb/stb_image.h"

std::array<std::unique_ptr<Texture>, 12> Texture::s_textures;

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

	m_buffer = stbi_load(path.c_str(), &m_width, &m_height, &m_nr_channels, 4);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_buffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (m_buffer)
		stbi_image_free(m_buffer);
	else
		std::cout << "Failed to load texture at \"" << path << "\"\n";

}

Texture::~Texture()
{
	glDeleteTextures(1, &m_id);
}

void Texture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::load_textures() {
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
}