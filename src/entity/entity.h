#pragma once
#include <unordered_map>
#include "../main/util.h"

class Entity
{
private:
	Point m_pos;
	Point m_velocity;
	bool m_collisions = false;
	int m_texture_id = 5;
public:

	static std::vector<Entity*> s_entities;
	static float s_size;
	float speed;

	Entity(Point, float);
	Entity(float, float, float);

	inline Point get_position() const { return m_pos; };
	inline Point get_velocity() const { return m_velocity; }
	inline int get_texture_id() const { return m_texture_id; }
	inline bool has_collisions() const { return m_collisions; };

	void set_position(Point);
	void set_position_x(float);
	void set_position_y(float);
	void set_velocity(Point);
	void set_velocity_x(float);
	void set_velocity_y(float);
	void set_texture_id(int);

	virtual ~Entity();

};