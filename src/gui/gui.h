#pragma once

#include <GLFW/glfw3.h>
#include <fstream>
#include "../stb/stb_truetype.h"
#include "../entity/registry.h"
#include "../entity/component.h"
#include "../main/texture.h"

void load_font(const char*);
void draw_text(float, float, const char*, float);
void gui_render(Registry&, Entity, float, float);
void subscreen_render(Registry&, float, float);
void settings_render(Registry&, float, float, bool&);
void hud_render(Registry&, Entity);

class Button 
{
private:
	Point m_pos;
	Point m_bounds;
	std::string m_text;
	const void*(*m_callback)();
public:

	Button(Point, Point, const std::string&, const void*(*)());
	Button(float, float, Point, const std::string&, const void*(*)());
	Button(Point, float, float, const std::string&, const void*(*)());
	Button(float, float, float, float, const std::string&, const void*(*)());

	inline Point get_position() { return m_pos; }
	inline Point get_bounds() { return m_bounds; }
	inline std::string get_text() { return m_text; }
	inline const void* (*get_callback())() { return m_callback; }

	void set_position(Point);
	void set_position(float, float);
	void set_bounds(Point);
	void set_bounds(float, float);
	void set_text(std::string&);

	virtual ~Button();

};