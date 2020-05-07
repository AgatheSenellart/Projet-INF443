#include "plants_modeling_functions.hpp"
using namespace vcl;
/*
mesh create_cone(float radius, float height, float z_offset){

    // Nomber of samples of the cone is N
    const size_t N = 20;

    mesh cone;
    cone.position.resize(N+2);

    // Fill tree geometry
    for(size_t kangle=0; kangle<N; ++kangle)
    {
            const float angle = kangle/(N-1.0f);

            // Compute coordinates
            cone.position[kangle + 2] = {cos(angle*2*3.14)*radius,sin(angle*2*3.14)*radius, z_offset};
    }
    cone.position[0]={0,0,z_offset};
    cone.position[1]={0,0,z_offset + height};

    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    for(unsigned int kangle=0; kangle<N-1; ++kangle)
    { const unsigned int idx = kangle + 2; // current vertex offset

            const uint3 triangle_1 = {idx, idx+1, 0};
            const uint3 triangle_2 = {idx, idx+1, 1};

            cone.connectivity.push_back(triangle_1);
            cone.connectivity.push_back(triangle_2);
    }

    return cone;
}

mesh create_cylinder(float radius, float height){

    // Nomber of samples of the cylinder is N*H
    const size_t N = 20;
    const size_t H = 2;

    mesh cylinder;
    cylinder.position.resize(N*H);

    // Fill tree geometry
    for(size_t kangle=0; kangle<N; ++kangle)
    {
        for(size_t kz=0; kz<H; ++kz)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float angle = kangle/(N-1.0f);
            const float z = kz/(H-1.0f);

            // Compute coordinates
            cylinder.position[kz*N + kangle] = evaluate_cylinder(angle,z,radius,height);
        }
    }


    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    for(unsigned int kangle=0; kangle<N-1; ++kangle)
    {
        for(unsigned int kz=0; kz<H-1; ++kz)
        {
            const unsigned int idx = N*kz + kangle; // current vertex offset

            const uint3 triangle_1 = {idx, idx+1+N, idx+1};
            const uint3 triangle_2 = {idx, idx+N, idx+1+N};

            cylinder.connectivity.push_back(triangle_1);
            cylinder.connectivity.push_back(triangle_2);
        }
    }

    return cylinder;
}

vec3 evaluate_cylinder(float angle, float z, float radius, float height)
{
    const float x = cos(angle*2*3.14)*radius;
    const float y = sin(angle*2*3.14)*radius;

    return {x,y,z*height};
}



mesh create_tree_foliage(float radius, float height, float z_offset)
{
    mesh m = create_cone(radius, height, 0);
    m.push_back( create_cone(radius, height, z_offset) );
    m.push_back( create_cone(radius, height, 2*z_offset) );

    return m;
}

void update_position(int N, std::vector<vcl::vec3>& positions, float min_distance, const gui_scene_structure& gui_scene){
    for (int i = 0; i < N ; i ++){
        bool far_enough = true;
        float u = rand_interval();
        float v = rand_interval();
        vec4 new_position_and_color = evaluate_terrain(u,v, gui_scene);
        float x = new_position_and_color[0];
        float y = new_position_and_color[1];
        float z = new_position_and_color[2]*new_position_and_color[3];
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
*/
