#include "entity.h"

std::vector<Entity*> Entity::s_entities = std::vector<Entity*>();
float Entity::s_size = 0.2f;

Entity::Entity(Point pos, float speed)
	: m_pos(pos), speed(speed)
{
	s_entities.push_back(this);
}

Entity::Entity(float x, float y, float speed)
	: m_pos(Point{x, y}), speed(speed)
{
	s_entities.push_back(this);
}

void Entity::set_position(Point pos) { m_pos = pos; }
void Entity::set_position_x(float x) { m_pos.x = x; }
void Entity::set_position_y(float y) { m_pos.y = y; }
void Entity::set_velocity(Point pos) { m_velocity = pos; }
void Entity::set_velocity_x(float x) { m_velocity.x = x; }
void Entity::set_velocity_y(float y) { m_velocity.y = y; }
void Entity::set_texture_id(int id) { m_texture_id = id; }

Entity::~Entity() = default;