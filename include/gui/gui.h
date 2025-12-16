#pragma once

#include <GLFW/glfw3.h>
#include <fstream>
#include <functional>
#include "../stb/stb_truetype.h"
#include "../entity/registry.h"
#include "../entity/component.h"
#include "../main/texture.h"

void load_font(const char*);
void draw_text(float, float, const char*, float);
void gui_render(std::shared_ptr<Registry>, Entity, float, float);
void subscreen_render(std::shared_ptr<Registry>, float, float);
void settings_render(std::shared_ptr<Registry>, float, float, bool&);
void hud_render(std::shared_ptr<Registry>, Entity);

class Button 
{
private:
	Point m_pos;
	Point m_bounds;
	std::string m_text{};
	const std::function<void()> m_callback = nullptr;
public:

	Button(Point pos, Point bounds, const std::string& text, const std::function<void()> callback)
		: m_pos(pos), m_bounds(bounds), m_text(text), m_callback(callback) {}
	Button(float x, float y, Point bounds, const std::string& text, const std::function<void()> callback)
		: m_pos({ x, y }), m_bounds(bounds), m_text(text), m_callback(callback) {}
	Button(Point pos, float width, float height, const std::string& text, const std::function<void()> callback)
		: m_pos(pos), m_bounds({ width, height }), m_text(text), m_callback(callback) {}
	Button(float x, float y, float width, float height, const std::string& text, const std::function<void()> callback)
		: m_pos({ x, y }), m_bounds({ width, height }), m_text(text), m_callback(callback) {}

	constexpr Point get_position() { return m_pos; }
	constexpr Point get_bounds() { return m_bounds; }
	constexpr std::string get_text() { return m_text; }
	const std::function<void()> get_callback() { return m_callback; }

	constexpr void set_position(Point pos) { m_pos = pos; }
	constexpr void set_position(float x, float y) { m_pos = { x, y }; }
	constexpr void set_bounds(Point bounds) { m_bounds = bounds; }
	constexpr void set_bounds(float width, float height) { m_bounds = { width, height }; }
	constexpr void set_text(std::string& text) { m_text = text; }

	virtual constexpr ~Button() = default;

};

class Slider
{
private:
	std::unordered_map<Point, std::string> m_notch{};
	std::string m_slider_text{};
	Point m_pos;
	Point m_bounds;
	Point m_slider_pos;
	Point m_slider_bounds;
	float m_min = 0.0f;
	float m_max = 1.0f;
public:

	Slider(Point pos, Point bounds)
		: m_pos(pos), m_bounds(bounds), m_slider_pos({ pos.x + bounds.x / 2, pos.y + bounds.y / 2 }), m_slider_bounds({ 5, 20 }) {}
	Slider(float x, float y, Point bounds)
		: m_pos({ x, y }), m_bounds(bounds), m_slider_pos({ x + bounds.x / 2, y + bounds.y / 2 }), m_slider_bounds({ 5, 20, }) {}
	Slider(Point pos, float width, float height)
		: m_pos(pos), m_bounds({ width, height }), m_slider_pos({ pos.x + width / 2, pos.y + height / 2 }), m_slider_bounds({ 5, 20 }) {}
	Slider(float x, float y, float width, float height)
		: m_pos({ x, y }), m_bounds({ width, height }), m_slider_pos({ x + width / 2, y + height / 2 }), m_slider_bounds({ 5, 20 }) {}

	constexpr Point get_position() { return m_pos; }
	constexpr Point get_bounds() { return m_bounds; }
	constexpr Point get_slider_position() { return m_slider_pos; }
	constexpr Point get_slider_bounds() { return m_slider_bounds; }
	constexpr float get_minimum() { return m_min; }
	constexpr float get_maximum() { return m_max; }
	constexpr std::string get_notch_text(Point pos) { return m_notch.find(pos)->second; }
	const auto& get_notch_map() { return m_notch; }
	Point get_notch_position(const std::string&);
	size_t get_notch_length() { return m_notch.size(); }

	void create_notch(Point pos, std::string& text) { m_notch.insert({ pos, text }); }
	void create_notch(float x, float y, std::string& text) { m_notch.insert({ { x, y }, text }); }

	constexpr void set_position(Point pos) { m_pos = pos; }
	constexpr void set_position(float x, float y) { m_pos = { x, y }; }
	constexpr void set_bounds(Point bounds) { m_bounds = bounds; }
	constexpr void set_bounds(float width, float height) { m_bounds = { width, height }; }
	constexpr void set_slider_position(Point pos) { m_slider_pos = pos; }
	constexpr void set_slider_position(float x, float y) { m_slider_pos = { x, y }; }
	constexpr void set_slider_bounds(Point bounds) { m_slider_bounds = bounds; }
	constexpr void set_slider_bounds(float width, float height) { m_slider_bounds = { width, height }; }
	constexpr void set_minimum(float min) { m_min = min; }
	constexpr void set_maximum(float max) { m_max = max; }
	constexpr void set_notch_text(Point pos, std::string& text) { m_notch.find(pos)->second = text; }
	constexpr void set_notch_text(float x, float y, std::string& text) { m_notch.find({ x, y })->second = text; }
	void set_notch_position(std::string&, Point);
	void set_notch_position(std::string&, float, float);

	virtual constexpr ~Slider() = default;

};

class Box
{
private:
	Point m_pos;
	Point m_bounds;
	bool m_toggled = false;
public:

	constexpr Box(Point pos, Point bounds)
		: m_pos(pos), m_bounds(bounds) {}
	constexpr Box(float x, float y, Point bounds)
		: m_pos({ x, y }), m_bounds(bounds) {}
	constexpr Box(Point pos, float width, float height)
		: m_pos(pos), m_bounds({ width, height }) {}
	constexpr Box(float x, float y, float width, float height)
		: m_pos({ x, y }), m_bounds({ width, height }) {}

	constexpr Point get_position() { return m_pos; }
	constexpr Point get_bounds() { return m_bounds; }
	constexpr bool is_toggled() { return m_toggled; }

	constexpr void set_position(Point pos) noexcept { m_pos = pos; };
	constexpr void set_position(float x, float y) noexcept { m_pos = { x, y }; };
	constexpr void set_bounds(Point bounds) noexcept { m_bounds = bounds; };
	constexpr void set_bounds(float width, float height) noexcept { m_bounds = { width, height }; };
	constexpr void set_toggled(bool toggle) noexcept { m_toggled = toggle; };

	virtual constexpr ~Box() = default;

};