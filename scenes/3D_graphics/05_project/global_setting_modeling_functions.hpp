#pragma once
#include "main/scene_base/base.hpp"
#include "project.hpp"
using namespace vcl;

// Stores some parameters that can be set from the GUI

vcl::mesh create_terrain(const gui_scene_structure& gui_scene);
vcl::vec3 evaluate_terrain(float u, float v);
