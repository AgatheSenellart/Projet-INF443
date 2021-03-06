#pragma once
#include "main/scene_base/base.hpp"
#include "tree.hpp"
#ifdef PROJECT
using namespace vcl;

struct gui_scene_structure
{
    bool wireframe = false;

    float height = 1.0f;
    float scaling = 6.0f;
    int octave = 2;
    float persistency = 0.5f;
};

// Structure of a particle
struct particle_structure
{
    vcl::vec3 p; // Position
    vcl::vec3 v; // Speed
    float scale;
    float alpha;
};

struct scene_model : scene_base
{

    /** A part must define two functions that are called from the main function:
     * setup_data: called once to setup data before starting the animation loop
     * frame_draw: called at every displayed frame within the animation loop
     *
     * These two functions receive the following parameters
     * - shaders: A set of shaders.
     * - scene: Contains general common object to define the 3D scene. Contains in particular the camera.
     * - data: The part-specific data structure defined previously
     * - gui: The GUI structure allowing to create/display buttons to interact with the scene.
    */

    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);

    //  List of class attributes to be initialized in setup_data and used in frame_draw
    mesh_drawable surface;
    mesh_drawable border;
    mesh_drawable water;
    mesh_drawable cliff;
    mesh_drawable waterfall_support;
    mesh_drawable moss;
    mesh_drawable reed;
    mesh_drawable wall;
    mesh_drawable bridge;
    mesh_drawable branche;
    mesh_drawable feuille;
    mesh_drawable sky;
    float wall_size;


    std::vector<vcl::vec3> moss_positions;
    std::vector<float> moss_sizes;
    std::vector<vcl::vec3> reed_positions;
    std::vector<vcl::vec3> tree_positions;

    //tree structure
    std::vector<noeud*> tree_structures;
    noeud* grand_arbre;

    // Id for the water texture
    GLuint channel0;
    GLuint channel1;
    GLuint channel2;

    //Id for the waterfall textures
    GLuint noise;
    GLuint facade;

    // textures' id for the hut
    GLuint wall_texture;
    GLuint wall_window_texture;
    GLuint wall_window_door_texture;
    GLuint roof_texture;

    // Keyboard interaction
    void keyboard_input(scene_structure& scene, GLFWwindow* window, int key, int scancode, int action, int mods);
    bool pedestrian_view;
    bool forward = false;
    camera_scene last_pedestrian_camera;
    camera_scene last_overview_camera;


    // Particles management
    std::list<particle_structure> particles; // Storage of all currently active particles
    vcl::mesh_drawable drop; // Visual representation of a particle - a sphere
    vcl::timer_event droplet_timer;    // Timer allowing to indicate periodic events



    void set_gui();

    // timer setup
    vcl::timer_interval water_timer;

    gui_scene_structure gui_scene;

};

#endif PROJECT
