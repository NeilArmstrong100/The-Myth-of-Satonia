#pragma once

#include <fstream>
#include <vector>
#include <GLFW/glfw3.h>
#include "../stb/stb_truetype.h"

void load_font(const char*);
void draw_text(float, float, const char*, float);