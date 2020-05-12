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
