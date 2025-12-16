#pragma once

#include <chrono>

#include "../entity/component.h"
#include "../entity/registry.h"
#include "../tile/tile.h"

void run();
void physics_update(const float);
void start(std::shared_ptr<Registry>);
void event_handler(float);
void horizontal_colliders(auto&, auto&, Transform&, Collision*, Point&, Point&, Point&, Entity);
void vertical_colliders(auto&, auto&, Transform&, Collision*, Point&, Point&, Point&, Entity);
void useable_event();
void chest_event();
void door_event();
void teleport_event();