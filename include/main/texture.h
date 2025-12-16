#pragma once

#include <GLFW/glfw3.h>
#include <string>
#include <print>
#include <array>
#include "util.h"

class Texture
{
private:
	std::string m_path{};
	unsigned int m_id = 0;
	int m_width = 0, m_height = 0, m_nr_channels = 0;
	std::byte* m_buffer = nullptr;
public:
	static inline std::array<std::unique_ptr<Texture>, 38> s_textures{};

	Texture(const std::string&);
	virtual ~Texture() { glDeleteTextures(1, &m_id); }

	void bind() const { glBindTexture(GL_TEXTURE_2D, m_id); }
	void unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

	constexpr int get_width() const { return m_width; }
	constexpr int get_height() const { return m_height; }

	static void load_textures();
};