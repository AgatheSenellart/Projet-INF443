#include "plants_modeling_functions.hpp"
#include "global_setting_modeling_functions.hpp"
using namespace vcl;
#ifdef PROJECT

void update_position_forest(int N, std::vector<vcl::vec3>& positions, float min_distance, const gui_scene_structure& gui_scene){
    for (int i = 0; i < N ; i ++){
        bool far_enough = true;
        float u = rand_interval();
        float v = rand_interval();

        float detect_river = v - 1.8f*u;
        float detect_sea = u - 0.5f*std::cos(4*v);
        float detect_cliff = v + 4.f/3.f*u - 0.2f;



        if (((-0.95 > detect_river) || -0.75 < (detect_river)) && (detect_sea > 0.5) && (detect_cliff < 1.5)){
            vec3 position = evaluate_terrain(u,v, gui_scene);
            float x = position[0];
            float y = position[1];
            float z = position[2];
            
            if (positions.size() > 0)
            {
                for (vec3 p : positions) {
                    float distance_2 = (x - p[0]) * (x - p[0]) + (y - p[1]) * (y - p[1]) + (z - p[2]) * (z - p[2]);
                    if (distance_2 < min_distance * min_distance) {
                        far_enough = false;
                    }
                }
            }
            if (far_enough){
                positions.push_back({x,y,z});
            }
        }
    }
}

void update_position_river(int N, std::vector<vcl::vec3>& positions, float min_distance, const gui_scene_structure& gui_scene){
    for (int i = 0; i < N ; i ++){
        bool far_enough = true;
        float u = rand_interval();
        float v = rand_interval();

        float detect_river = v - 1.8*u;
        float detect_sea = u - 0.5f*std::cos(4*v);

        bool on_bank = ((-0.95 < detect_river) && (-0.9 > detect_river) ) || ((-0.8 < detect_river) && (-0.75> detect_river)) || ((0.5f < detect_sea) && (0.6f> detect_sea));

        if ( on_bank && (detect_sea > 0.5f) && ((detect_river < -0.9) || (detect_river > -0.8))){
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
    mesh moss = mesh_primitive_sphere(0.03, {0,0,0}, 100, 100);
    return moss;
    }

mesh create_reed(){
    mesh reed;
    reed.position     = {{-0.2f,0,0}, { 0.2f,0,0}, { 0.2f, 0.4f,0}, {-0.2f, 0.4f,0}};
    reed.texture_uv   = {{0,1}, {1,1}, {1,0}, {0,0}};
    reed.connectivity = {{0,1,2}, {0,2,3}};

    return reed;
}
#endif
