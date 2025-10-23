#include "tile.h"

std::unordered_map<Point, Tile> Tile::s_tiles = std::unordered_map<Point, Tile>();
float Tile::s_size = 0.2f;

Tile::Tile(Point pos, byte id)
	: m_pos(pos), m_id(id)
{
	s_tiles.emplace(m_pos, *this);
}

Tile::Tile(float x, float y, byte id)
	: m_pos(Point{x, y}), m_id(id)
{
	s_tiles.emplace(m_pos, *this);
}

void Tile::set_position(Point pos) { m_pos = pos; }
void Tile::set_position_x(float x) { m_pos.x = x; }
void Tile::set_position_y(float y) { m_pos.y = y; }

Tile::~Tile() = default;