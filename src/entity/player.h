#pragma once
#include "entity.h"

class Player : public Entity
{
	bool m_collisions = true;
	using Entity::Entity;
};