#include "global_setting_modeling_functions.hpp"
using namespace vcl;


// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
vec3 evaluate_terrain(float u, float v)
{
    const float x = 20*(u-0.5f);
    const float y = 20*(v-0.5f);

    float z = 0;

    float detect_river = v - 1.8*u;
    float river_depth = 5.0;
    if (-1. < detect_river){
        if (-0.8 > detect_river){
                float distance = std::min(std::abs(-1. - detect_river), std::abs(detect_river + 0.8));
                z -= river_depth*std::sqrt(distance);
        }
    }

    float detect_sea = u - std::cos(4*v);
    float sea_depth = 4.0;
    if (detect_sea < 0){
        z -= sea_depth * std::sqrt(- detect_sea);
    }

    if (-1. < detect_river){
        if (-0.8 > detect_river){
             if (detect_sea < 0){
                 float distance = std::min(std::abs(-1. - detect_river), std::abs(detect_river + 0.8));
                 float a = river_depth*std::sqrt(distance);
                 float b = sea_depth * std::sqrt(- detect_sea);
                 z = -std::max(a, b);
             }
        }
     }

    return {x,y,z};
}

// Generate terrain mesh
mesh create_terrain(const gui_scene_structure& gui_scene)
{
    // Number of samples of the terrain is N x N
    const size_t N = 100;

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N*N);
    terrain.texture_uv.resize(N*N);


    // Fill terrain geometry
    for(size_t ku=0; ku<N; ++ku)
    {
        for(size_t kv=0; kv<N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku/(N-1.0f);
            const float v = kv/(N-1.0f);


            // Compute coordinates and texture and color
            vec3 coordinates = evaluate_terrain(u,v);
            float x  = coordinates[0];
            float y = coordinates[1];
            float z = coordinates[2];

            // get gui parameters
            const float scaling = gui_scene.scaling;
            const int octave = gui_scene.octave;
            const float persistency = gui_scene.persistency;
            const float height = gui_scene.height;

            // Evaluate Perlin noise
            const float noise = perlin(scaling*u, scaling*v, octave, persistency);

            float detect_river = v - 1.8*u;
            float detect_sea = u - std::cos(4*v);

            terrain.position[kv+N*ku] = {x, y, 0.7*z + 0.3*noise};
            terrain.texture_uv[kv+N*ku] = {5*u,5*v};
        }
    }


    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    const unsigned int Ns = N;
    for(unsigned int ku=0; ku<Ns-1; ++ku)
    {
        for(unsigned int kv=0; kv<Ns-1; ++kv)
        {
            const unsigned int idx = kv + N*ku; // current vertex offset

            const uint3 triangle_1 = {idx, idx+1+Ns, idx+1};
            const uint3 triangle_2 = {idx, idx+Ns, idx+1+Ns};

            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }
    }

    return terrain;
}

mesh create_cliff(){

    const size_t size_A = 30;
    const float size_Af = 30.0;
    const size_t size_B = 10;
    const float size_Bf = 10.0;
    const size_t size_C = 50;
    const float size_Cf = 50.0;
    const size_t size_D = 50;
    const float size_Df = 50.0;
    const size_t H = 30;
    const float Hf = 30.0;
    const size_t total = size_A + size_B + size_C + size_D;

    mesh cliff;
    cliff.position.resize(total*H);

    // Fill tree geometry
    for(size_t kz = 0; kz < H; ++kz){
        for (unsigned int i = 0; i < total; i++){
            if (i < size_A){ //side_A
                cliff.position[kz*total + i] = {- i / size_Af*0.3, 0, kz/Hf};
            }
            if ((i >= size_A) && (i < size_A + size_B)){//side B
                cliff.position[kz*total + i] = {-0.3, - (i-size_Af)*0.1 / size_Bf, kz/Hf};
            }
            if ((i >= size_A + size_B) && (i < size_A + size_B + size_C)){//side C
                float x = - 0.3 - 0.3*(i - size_Af - size_Bf) / size_Cf;
                float y = -4./3.*x - 0.5;
                cliff.position[kz*total + i] = {x, y, kz/Hf};
            }
            if (i >= size_A + size_B + size_C){//side D
                cliff.position[kz*total + i] = {0, -0.5 + 0.5*(i - size_Af - size_Bf - size_Cf)/size_Df, kz/Hf};
            }
        }
    }


    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    for(unsigned int kz = 0 ; kz < H-1 ; kz++){
        for(unsigned int i=0; i<total-1; i++){
            const unsigned int idx = total*kz + i; // current vertex offset

            const uint3 triangle_1 = {idx, idx+1+total, idx+1};
            const uint3 triangle_2 = {idx, idx+total, idx+1+total};

            cliff.connectivity.push_back(triangle_1);
            cliff.connectivity.push_back(triangle_2);
        }
    }

    return cliff;
}
