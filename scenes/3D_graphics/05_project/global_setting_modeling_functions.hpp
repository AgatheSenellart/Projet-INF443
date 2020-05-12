#include "main/scene_base/base.hpp"
#include "project.hpp"
using namespace vcl;

vcl::mesh create_terrain(const gui_scene_structure& gui_scene);
vcl::vec3 evaluate_terrain(float u, float v, const gui_scene_structure& gui_scene);
mesh create_water(const gui_scene_structure& gui_scene);
mesh create_cliff();
