#pragma once
#include <unordered_map>
#include "util.h"
#include "texture.h"

class Tile
{
private:
	Point m_pos;
	bool m_collisions = false;
	const byte m_id;
public:

	static std::unordered_map<Point, Tile> s_tiles;
	static float s_size;

	Tile(Point, byte);
	Tile(float, float, byte);

	inline Point get_position() const { return m_pos; }
	inline byte get_id() const { return m_id; }
	inline bool has_collisions() const { return m_collisions; }

	void set_position(Point);
	void set_position_x(float);
	void set_position_y(float);

	virtual ~Tile();

};