#pragma once

#include<GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <array>
#include "util.h"

class Texture
{
private:
	unsigned int m_id;
	std::string m_path;
	byte* m_buffer;
	int m_width, m_height, m_nr_channels;
public:
	static std::array<std::unique_ptr<Texture>, 16> s_textures;

	Texture(const std::string&);
	~Texture();

	void bind() const;
	void unbind() const;

	inline int get_width() const { return m_width; }
	inline int get_height() const { return m_height; }

	static void load_textures();
};