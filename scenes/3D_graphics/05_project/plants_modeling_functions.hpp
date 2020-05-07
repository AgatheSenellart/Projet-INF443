#pragma once
#include "main/scene_base/base.hpp"
#include "project.hpp"
using namespace vcl;

vec3 evaluate_cylinder(float u, float v, float radius, float height);
mesh create_cylinder(float radius, float height);
mesh create_cone(float radius, float height, float z_offset);
mesh create_tree_foliage(float radius, float height, float z_offset);
void update_position(int N, std::vector<vcl::vec3>& position, float min_distance, const gui_scene_structure& gui_scene);
