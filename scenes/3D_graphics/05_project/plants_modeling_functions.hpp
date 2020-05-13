#include "main/scene_base/base.hpp"
#include "project.hpp"

using namespace vcl;


void update_position_forest(int N, std::vector<vcl::vec3>& positions, float min_distance, const gui_scene_structure& gui_scene);
void update_position_river(int N, std::vector<vcl::vec3>& positions, float min_distance, const gui_scene_structure& gui_scene);
void update_size(int N, std::vector<float>& sizes);
mesh create_moss();
mesh create_reed();

