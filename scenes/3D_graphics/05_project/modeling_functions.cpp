#include "modeling_functions.hpp"
using namespace vcl;

// Evaluate height of the terrain for any (u,v) \in [0,1]
float evaluate_terrain_z(float u, float v)
{
/*    const vec4 p = {(0,0),(0.5f,0.5f),(0.2f,0.7f),(0.8f,0.7f)};
    const vec4 h = {3.0f,-1.5f,1.0f,2.0f};
    const vec4 sigma = {0.5f,0.15f,0.2f,0.2f};

    float d;
    float z = 0;

    for (int i = 0; i < 4; i ++){
        d = norm(vec2(u,v)-p[i])/sigma[i];
        z += h[i]*std::exp(-d*d);
    }
*/

    float z = 0;
    float detect_river = v - 1.8*u;
    float profondeur = 4.0;

    if (-1.0 < detect_river){
        if (-0.9 > detect_river){
            float distance = std::min(std::abs(-1.0 - detect_river), std::abs(detect_river - 0.9));
            z -= profondeur*(1-std::exp(-distance));
        }
    }

    return z;
}

// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
vec3 evaluate_terrain(float u, float v)
{
    const float x = 20*(u-0.5f);
    const float y = 20*(v-0.5f);
    const float z = evaluate_terrain_z(u,v);

    return {x,y,z};
}

// Generate terrain mesh
mesh create_terrain()
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
            terrain.position[kv+N*ku] = {coordinates[0], coordinates[1], coordinates[2]};
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

