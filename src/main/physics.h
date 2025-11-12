#pragma once
#include "../entity/component.h"
#include "../entity/registry.h"
#include "../tile/tile.h"

void run();
void physics_update(const float);
void start(Registry&);
void event_handler(float);