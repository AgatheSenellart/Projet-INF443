#include "global_setting_modeling_functions.hpp"
using namespace vcl;


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

            terrain.position[kv+N*ku] = evaluate_terrain(u,v, gui_scene);
            terrain.texture_uv[kv+N*ku] = {2*u,2*v};
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

// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
vec3 evaluate_terrain(float u, float v, const gui_scene_structure& gui_scene)
{
    const float x = 20*(u-0.5f);
    const float y = 20*(v-0.5f);

    float z = 0;

    float detect_river = v - 1.8*u;
    float river_depth = 4.0;
    if (-0.9 < detect_river){
        if (-0.8 > detect_river){
                float distance = std::min(std::abs(-0.9 - detect_river), std::abs(detect_river + 0.8));
                z -= river_depth*std::sqrt(distance);
        }
    }

    float detect_sea = u - std::cos(4*v);
    float sea_depth = 4.0;
    if (detect_sea < 0){
        z -= sea_depth * std::sqrt(- detect_sea);
    }

    if (-0.9 < detect_river){
        if (-0.8 > detect_river){
             if (detect_sea < 0){
                 float distance = std::min(std::abs(-0.9 - detect_river), std::abs(detect_river + 0.8));
                 float a = river_depth*std::sqrt(distance);
                 float b = sea_depth * std::sqrt(- detect_sea);
                 z = -std::max(a, b);
             }
        }
     }

    // get gui parameters
    const float scaling = gui_scene.scaling;
    const int octave = gui_scene.octave;
    const float persistency = gui_scene.persistency;
    const float height = gui_scene.height;

    // Evaluate Perlin noise
    const float noise = perlin(scaling*u, scaling*v, octave, persistency);


    return {x,y,0.7f*z + 0.3f*noise};
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
    cliff.position.resize(total*(H+1) + (size_A - 1)*(size_B-1));
    cliff.texture_uv.resize(total*(H+1) + (size_A - 1)*(size_B-1));

    // Fill sides geometry
    for(size_t kz = 0; kz < H+1; ++kz){
        for (unsigned int i = 0; i < total; i++){
            float kz_texture = 0.03*kz;
            float i_texture = 0.03*i;
            if (i < size_A){ //side_A
                cliff.position[kz*total + i] = {- (i/size_Af)*0.3f, 0, 0.3f*kz/Hf};
                cliff.texture_uv[kz*total + i] = {kz_texture,i_texture};
            }
            if ((i >= size_A) && (i < size_A + size_B)){//side B
                cliff.position[kz*total + i] = {-0.3f, - (i-size_Af)*0.1f / size_Bf, 0.3f*kz/Hf};
                cliff.texture_uv[kz*total + i] = {kz_texture,i_texture};
            }
            if ((i >= size_A + size_B) && (i < size_A + size_B + size_C)){//side C
                float x = - 0.3 + 0.3*(i - size_Af - size_Bf) / size_Cf;
                float y = -4./3.*x - 0.5;
                cliff.position[kz*total + i] = {x, y, 0.3f*kz/Hf};
                cliff.texture_uv[kz*total + i] = {kz_texture,i_texture};
            }
            if (i >= size_A + size_B + size_C){//side D
                cliff.position[kz*total + i] = {0, -0.5f + 0.5f*(i - size_Af - size_Bf - size_Cf)/size_Df, 0.3f*kz/Hf};
                cliff.texture_uv[kz*total + i] = {kz_texture,i_texture};
            }
        }
    }

    // Fill top geometry
    for(size_t kx = 0; kx < size_A - 1; ++kx){
        for (size_t ky = 0; ky < size_B - 1; ++ky){
                cliff.position[total*(H+1) + ky*(size_A - 1) + kx] = {- ((kx + 1)/size_Af)*0.3f, - ((ky + 1)/size_Bf)*0.1f, 0.3f*Hf};
                cliff.texture_uv[total*(H+1) + ky*(size_A - 1) + kx] = {0.03f*kx,0.03f*ky};
            }
    }


    // Generate triangle organization
    //  Fill sides connectivity
    for(unsigned int kz = 0 ; kz < H ; kz++){
        for(unsigned int i=0; i<total-1; i++){
            const unsigned int idx = total*kz + i; // current vertex offset

            const uint3 triangle_1 = {idx, idx+1+total, idx+1};
            const uint3 triangle_2 = {idx, idx+total, idx+1+total};

           cliff.connectivity.push_back(triangle_1);
           cliff.connectivity.push_back(triangle_2);
        }
       cliff.connectivity.push_back({total*kz, total*kz + total - 1, (kz+1)*total + total - 1});
       cliff.connectivity.push_back({total*kz,total*(kz+1) , (kz+1)*total + total - 1});
    }

    //  Fill top connectivity
    for(size_t kx = 0; kx < size_A - 1; ++kx){
        for (size_t ky = 0; ky< size_B - 1; ++ky){
            }
    }


    //cliff.connectivity.push_back({total*H, total*H + size_A, total*H + size_A + size_B});
    //cliff.connectivity.push_back({total*H + size_A + size_B,total*H + size_A + size_B + size_C, total*H});

    return cliff;
}


// Generate terrain mesh
mesh create_water(const gui_scene_structure& gui_scene)
{
    // Number of samples of the terrain is N x N
    const size_t N = 100;

    mesh water; // temporary terrain storage (CPU only)
    water.position.resize(N*N);

    // Fill terrain geometry
    for(size_t ku=0; ku<N; ++ku)
    {
        for(size_t kv=0; kv<N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku/(N-1.0f);
            const float v = kv/(N-1.0f);

            water.position[kv+N*ku] = {20*(u-0.5f), 20*(v-0.5f), 0.0f};
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

            water.connectivity.push_back(triangle_1);
            water.connectivity.push_back(triangle_2);
        }
    }

    return water;
}
