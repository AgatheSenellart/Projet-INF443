#include "human_activity_modeling_functions.hpp"
#include "global_setting_modeling_functions.hpp"
using namespace vcl;

mesh create_wall(float taille){
    mesh wall;
    wall.position     = {{0.0f,0,0}, { 0.0f,0,taille}, { taille, 0.0f,0.0}, {taille, 0.0f,taille}};
    wall.texture_uv   = {{1,1}, {1,0},{0,1},{0,0}};
    wall.connectivity = {{0,1,2}, {1,2,3}};

    return wall;
}

mesh create_bridge(float h, float L)
{
    // Number of samples of the terrain is N x N
    const size_t N = 30;
    const float Nf = 30.0f;
    const size_t Y = 3;
    const float Yf = 3.0f;

    mesh bridge; // temporary terrain storage (CPU only)
    bridge.position.resize((N+1)*Y);
    bridge.texture_uv.resize((N+1)*Y);


    // Fill terrain geometry
    for(size_t ky=0; ky<Y; ++ky)
    {
        for(size_t kn=0; kn<N + 1; ++kn)
        {
            float x = (kn/Nf)*L;
            float z = -(4*h/std::pow(L,2))*pow(x,2) + (4*h/L)*x;
            bridge.position[(N+1)*ky+kn] = {x,(ky/Yf)*h,z};
            bridge.texture_uv[(N+1)*ky+kn] = {1.0f*ky,1.0f*kn};
        }
    }

    // Generate triangle organization
    for(unsigned int ky=0; ky<Y-1; ++ky)
    {
        for(unsigned int kn=0; kn<N; ++kn)
        {
            const unsigned int idx = (N+1)*ky+kn; // current vertex offset

            const uint3 triangle_1 = {idx, idx+1+(N+1), idx+1};
            const uint3 triangle_2 = {idx, idx+(N+1), idx+1+(N+1)};

            bridge.connectivity.push_back(triangle_1);
            bridge.connectivity.push_back(triangle_2);
        }
    }

    return bridge;
}
