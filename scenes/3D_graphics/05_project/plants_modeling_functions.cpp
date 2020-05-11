#include "plants_modeling_functions.hpp"
#include "global_setting_modeling_functions.hpp"
using namespace vcl;

void update_position(int N, std::vector<vcl::vec3>& positions, float min_distance, const gui_scene_structure& gui_scene){
    for (int i = 0; i < N ; i ++){
        bool far_enough = true;
        float u = rand_interval();
        float v = rand_interval();

        float detect_river = v - 1.8*u;
        float detect_sea = u - std::cos(4*v);



        if (((-0.9 > detect_river) || -0.8 < (detect_river)) && (detect_sea > 0)){
            vec3 position = evaluate_terrain(u,v, gui_scene);
            float x = position[0];
            float y = position[1];
            float z = position[2];
            for (vec3 position : positions){
                float distance_2 = (x-position[0])*(x-position[0]) + (y-position[1])*(y-position[1]) + (z-position[2])*(z-position[2]);
                if (distance_2 < min_distance*min_distance){
                    far_enough = false;
                }
            }
            if (far_enough){
                positions.push_back({x,y,z});
            }
        }
    }
}



void update_size(int N, std::vector<float>& sizes){
    for (int i = 0; i < N ; i ++){
        float s = rand_interval();
        float size = 0.5+s;
        sizes.push_back(size);
            }
        }

mesh create_moss(){
    mesh moss = mesh_primitive_sphere(0.05, {0,0,0}, 100, 100);
    return moss;
    }
