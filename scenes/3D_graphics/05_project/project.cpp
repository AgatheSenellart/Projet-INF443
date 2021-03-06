#include "project.hpp"
#include "global_setting_modeling_functions.hpp"
#include "plants_modeling_functions.hpp"
#include "human_activity_modeling_functions.hpp"
#include "tree.hpp"

#ifdef PROJECT
#include <random>
// Generator for uniform random number
std::default_random_engine generator;
std::uniform_real_distribution<float> distrib(0.0,1.0);

using namespace vcl;

/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_model::setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& ){

    // Create visual terrain surface
    surface = create_terrain(gui_scene);
    surface.uniform.color = {0.7f,0.9f,0.7f};
    surface.uniform.shading.specular = 0.0f; // non-specular terrain material
    surface.texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/05_project/assets/forest_floor_2.png"));

    //Create border
    border = create_border();
    border.uniform.color = {1.0f, 1.0f, 1.0f};
    border.uniform.shading.diffuse = 1.0;
    border.uniform.shading.specular = 0.0;
    border.uniform.shading.ambiant = 1.0f;

    // Setup initial camera mode and position
    scene.camera.camera_type = vcl::camera_control_spherical_coordinates;
    scene.camera.scale = 20.0f;
    scene.camera.apply_rotation(-1.0f,0,0,1.2f);

    //Setup initial views position
    pedestrian_view = false;
    last_overview_camera = scene.camera;
    last_pedestrian_camera = scene.camera;
    last_pedestrian_camera.translation = vec3(-3.0f,0.0f,-1.5f);
    last_pedestrian_camera.scale = 0.1f;
    last_pedestrian_camera.orientation = mat3{{1,0,0},{0,0,1},{0,-1,0}};


    //Create cliff
    cliff = create_cliff(gui_scene);
    cliff.uniform.color = {1.0f,1.0f,1.0f};
    cliff.uniform.shading.ambiant = 0.8f;
    cliff.uniform.shading.specular = 0.0f;
    cliff.texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/05_project/assets/cliff_4.png"));
    waterfall_support = create_waterfall_support(gui_scene);
    waterfall_support.uniform.color = {1.0f,1.0f,1.0f};
    waterfall_support.uniform.shading.ambiant = 0.8f;
    waterfall_support.uniform.shading.specular = 0.0f;

    //Create water

    // Id for the multitexturing of the water
    channel0 = create_texture_gpu(image_load_png("scenes/3D_graphics/05_project/assets/channel0.png"));
    channel1 = create_texture_gpu(image_load_png("scenes/3D_graphics/05_project/assets/channel1.png"));
    channel2 = create_texture_gpu(image_load_png("scenes/3D_graphics/05_project/assets/channel2.png"));


    water = create_water(gui_scene);
    water.uniform.color = {0.2f, 0.6, 1.0};
    
    // Create drop
    drop = create_drop();
    drop.texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/05_project/assets/droplet_4.png"), GL_REPEAT, GL_REPEAT);
    droplet_timer.periodic_event_time_step = 0.001f;


    //Create moss
    moss = create_moss();
    surface.uniform.color = {0.7f,0.9f,0.7f};
    moss.uniform.shading.specular = 0.0f; // non-specular terrain material
    moss.texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/05_project/assets/moss.png"));
    update_position_forest(500,moss_positions,0, gui_scene);
    update_size(500, moss_sizes);

    //Create wall
    wall_size = 1.0;
    wall = create_wall(wall_size);
    wall.uniform.shading.ambiant = 0.6f;
    wall_texture = create_texture_gpu(image_load_png("scenes/3D_graphics/05_project/assets/cabane_mur_simple.png"));
    wall_window_texture = create_texture_gpu(image_load_png("scenes/3D_graphics/05_project/assets/cabane_fenetre.png"));
    wall_window_door_texture = create_texture_gpu(image_load_png("scenes/3D_graphics/05_project/assets/cabane_porte_fenetre.png"));
    roof_texture = create_texture_gpu(image_load_png("scenes/3D_graphics/05_project/assets/toit.png"));

    //Create bridge
    bridge = create_bridge(1.0f, 2.5f);
    bridge.uniform.shading.ambiant = 0.6f;

    //Create reed
    reed = create_reed();
    reed.uniform.shading = {0.8f,0,0}; // set pure ambiant component (no diffuse, no specular) - allow to only see the color of the texture
    reed.uniform.transform.scaling = 0.5;
    reed.texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/05_project/assets/billboard_reed_2.png"), GL_REPEAT, GL_REPEAT);
    update_position_river(1500, reed_positions, 0, gui_scene);

    //Create visual elements for the trees
    float taille_branche = 0.15f;
    branche = vcl::branche(taille_branche);
    feuille = vcl::feuille(taille_branche);
    branche.shader = shaders["mesh"]; feuille.shader = shaders["mesh"];
    update_position_forest(200, tree_positions ,2*taille_branche, gui_scene);
    //Structure des arbres
    for (int i = 0; i < 10; i++)
    {
        noeud* arbre = grammar_tree(4);
        fill_coordinates(arbre, taille_branche);
        tree_structures.push_back(arbre);
    }
    // a bigger tree next to the house
    
    grand_arbre = grammar_tree(5);
    fill_coordinates(grand_arbre, taille_branche);

    water_timer.t_max = 50;

    //Create Skybox
    sky = skybox();
    sky.texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/05_project/assets/sky3.png"));
    
    //Create texture_id for the waterfall
    noise = create_texture_gpu(image_load_png("scenes/3D_graphics/05_project/assets/noise.png"));
    facade = create_texture_gpu(image_load_png("scenes/3D_graphics/05_project/assets/waterfall_support.png"));


}   


/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& ){

    const float dt = droplet_timer.update();
    water_timer.update();
    // Get local time
    float t = water_timer.t;

    set_gui();
   

    glEnable( GL_POLYGON_OFFSET_FILL ); // avoids z-fighting when displaying wireframe


    // Display terrain
    glPolygonOffset( 1.0, 1.0 );
    glBindTexture(GL_TEXTURE_2D, surface.texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    draw(surface, scene.camera, shaders["mesh"]);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);

    //Walking in pedestrian view
    if (forward){
        scene.camera.apply_translation_orthogonal_to_screen_plane(0.2);
        vec3 coords = scene.camera.translation;
        scene.camera.translation = vec3(coords[0], coords[1], - 1.0f);
    }

    //Display border
    border.uniform.transform.rotation = {{1.f,0.f,0.f},{0.f,1.f,0.f},{0.f,0.f,1.f}};
    border.uniform.transform.translation = {0.f,10.f,-1.8f};
    //draw(border, scene.camera, shaders["mesh"]);
    border.uniform.transform.translation = {0.f,0.f,0.f};
    border.uniform.transform.rotation = {{1.f,0.f,0.f},{0.f,1.f,0.f},{0.f,0.f,1.f}};
    border.uniform.transform.translation = {0.f,-10.f,-1.8f};
    //draw(border, scene.camera, shaders["mesh"]);
    border.uniform.transform.translation = {0.f,0.f,0.f};
    border.uniform.transform.rotation = {{0.f,1.f,0.f},{-1.f,0.f,0.f},{0.f,0.f,1.f}};
    border.uniform.transform.translation = {-10.f,0,-1.8f};
    //draw(border, scene.camera, shaders["mesh"]);


    // Display cliff
    gui_scene.scaling = 4.f;
    gui_scene.octave = 9;
    gui_scene.persistency = 0.7f;

    glBindTexture(GL_TEXTURE_2D, cliff.texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    cliff.uniform.transform.scaling = 50;
    cliff.uniform.transform.translation = vec3(20, 20, -1);
    draw(cliff, scene.camera, shaders["mesh"]);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);

    // Display water
    //channel0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, channel0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    //channel1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, channel1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    //channel2
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, channel2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    //Activate Shader
    glUseProgram(shaders["river"]);


    //Send the time to the shader;
    const GLint time_river = glGetUniformLocation(shaders["river"], "time");
    glUniform1f(time_river, t);

    //Send texture units to the shader;

    const GLint channel0_loc = glGetUniformLocation(shaders["river"], "iChannel0");

    glUniform1i(channel0_loc, 0);
    const GLint channel1_loc = glGetUniformLocation(shaders["river"], "iChannel1");

    glUniform1i(channel1_loc, 1);
    const GLint channel2_loc = glGetUniformLocation(shaders["river"], "iChannel2");

    glUniform1i(channel2_loc, 2);


    //draw(water, scene.camera,shaders["river"]);

    // put neutral texture again
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    glActiveTexture(GL_TEXTURE0);



    // Display waterfall
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(false);
        // Adjust some uniform parameters
    waterfall_support.uniform.transform.scaling = 50;
    waterfall_support.uniform.transform.translation = vec3(19.5, 19.5, -1);
        // Bind the textures necessary
    //noise
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, noise);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    //facade
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, facade);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    //Activate Shader
    glUseProgram(shaders["waterfall"]);

    //Send the time to the shader;
    const GLint time_waterfall = glGetUniformLocation(shaders["waterfall"], "time");
    glUniform1f(time_waterfall, t);

    //Send texture units to the shader;

    const GLint noise_loc = glGetUniformLocation(shaders["waterfall"], "noise");
    glUniform1i(noise_loc, 0);
    const GLint facade_loc = glGetUniformLocation(shaders["waterfall"], "falaise");
    glUniform1i(facade_loc, 1);

    draw(waterfall_support, scene.camera, shaders["waterfall"]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    glDepthMask(true);



    // Emission of new particle if needed
    const bool is_new_particle = droplet_timer.event;
    if( is_new_particle )
    {
        const vec3 p0 = {12.f,4.f,-1.0f};

        for (int i=0; i<1; i++){
            // Initial speed is random. (x,z) components are uniformly distributed along a circle.
            const float theta     = 2*3.14f*distrib(generator);
            const vec3 v0 = vec3( 1.2*std::cos(theta), 1.2*std::sin(theta), 5.0f);

            particles.push_back({p0,v0,3.0f,1.0f});

        }
    }


    // Evolve position of particles
    const vec3 g = {0.0f,0.0f,-9.81f};
    for(particle_structure& particle : particles)
    {
        const float m = 0.01f; // particle mass

        vec3& p = particle.p;
        vec3& v = particle.v;

        const vec3 F = m*g;

        // Numerical integration
        v = v + dt*F/m;
        p = p + dt*v;

        // Evolve size and transparency of the smoke
        particle.scale *= 1.15;
        particle.alpha /= 1.1;
    }

    if (particles.size() > 1) {
        // Remove particles that are too low
        for (auto it = particles.begin(); it != particles.end(); ++it) {
            if (it->p.z < -5)
                it = particles.erase(it);
        }
        
    }

    // Display particles

    glEnable(GL_BLEND); // Enable use of alpha component as color blending for transparent elements
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable depth buffer writing
    //  - Transparent elements cannot use depth buffer
    //  - They are supposed to be display from furest to nearest elements
    glDepthMask(false);

    glBindTexture(GL_TEXTURE_2D, drop.texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // avoids sampling artifacts

    for(particle_structure& particle : particles){
        drop.uniform.transform.rotation = scene.camera.orientation;
        drop.uniform.transform.translation = particle.p;
        drop.uniform.transform.scaling = particle.scale;
        drop.uniform.color_alpha = particle.alpha;
        drop.uniform.color = {1, 1, 1 };
        draw(drop, scene.camera, shaders["mesh"]);
        }

    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    glDepthMask(true);

    // Display bridge
    glBindTexture(GL_TEXTURE_2D, roof_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    bridge.uniform.transform.scaling = 1.3;
    bridge.uniform.transform.translation = {7.2f, -2.0f, -0.1f};
    float bridge_angle = 3.14/6;
    bridge.uniform.transform.rotation = {{cos(bridge_angle), -sin(bridge_angle), 0}, {sin(bridge_angle), cos(bridge_angle), 0}, {0.0f, 0.0f, 1.0f}};
    draw(bridge, scene.camera, shaders["mesh"]);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);


    // Display moss
    glBindTexture(GL_TEXTURE_2D, moss.texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    for (float i = 0 ; i < moss_positions.size() ; i ++){
        moss.uniform.transform.scaling = moss_sizes[i];
        moss.uniform.transform.translation = moss_positions[i];
        draw(moss, scene.camera, shaders["mesh"]);
    }

    glBindTexture(GL_TEXTURE_2D, scene.texture_white);

    //Display hut

    float angle = -3.14/6;

    glBindTexture(GL_TEXTURE_2D, wall_window_door_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    wall.uniform.transform.rotation = {{std::cos(angle), -std::sin(angle), 0.0}, {std::sin(angle), std::cos(angle), 0.0},{0.0, 0.0, 1.0}};
    wall.uniform.transform.translation = evaluate_terrain(0.2, 0.8, gui_scene) + vec3(0.0f, 0.0f, -0.4f);
    draw(wall, scene.camera, shaders["mesh"]);

    glBindTexture(GL_TEXTURE_2D, wall_window_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    angle = angle - 3.14/2;
    wall.uniform.transform.rotation = {{std::cos(angle), -std::sin(angle), 0.0}, {std::sin(angle), std::cos(angle), 0.0},{0.0, 0.0, 1.0}};
    wall.uniform.transform.translation = evaluate_terrain(0.2, 0.8, gui_scene) + vec3(0.0f, 0.0f, -0.4f);
    draw(wall, scene.camera, shaders["mesh"]);

    glBindTexture(GL_TEXTURE_2D, wall_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    wall.uniform.transform.rotation = {{std::cos(angle), -std::sin(angle), 0.0}, {std::sin(angle), std::cos(angle), 0.0},{0.0, 0.0, 1.0}};
    wall.uniform.transform.translation = evaluate_terrain(0.2f, 0.8f, gui_scene) + vec3(0.0f, 0.0f, -0.4f) + vec3({std::cos(3.14f/6)*wall_size, std::sin(3.14f/6)*wall_size, 0});
    draw(wall, scene.camera, shaders["mesh"]);

    angle = angle + 3.14/2;
    wall.uniform.transform.rotation = {{std::cos(angle), -std::sin(angle), 0.0}, {std::sin(angle), std::cos(angle), 0.0},{0.0, 0.0, 1.0}};
    wall.uniform.transform.translation = evaluate_terrain(0.2f, 0.8f, gui_scene) + vec3(0.0f, 0.0f, -0.4f) + vec3({-std::cos(3.14f/3)*wall_size, std::sin(3.14f/3)*wall_size, 0});
    draw(wall, scene.camera, shaders["mesh"]);

    glBindTexture(GL_TEXTURE_2D, roof_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    wall.uniform.transform.rotation = {{std::cos(angle), -std::sin(angle), 0.0}, {0, 0, 1.0},{std::sin(angle),std::cos(angle), 0}};
    wall.uniform.transform.translation = evaluate_terrain(0.2, 0.8, gui_scene) + vec3({0.0, 0.0,wall_size}) + vec3(0.0f, 0.0f, -0.4f);
    draw(wall, scene.camera, shaders["mesh"]);


    glBindTexture(GL_TEXTURE_2D, scene.texture_white);


    // Diplay reed

    glEnable(GL_BLEND); // Enable use of alpha component as color blending for transparent elements
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable depth buffer writing
    //  - Transparent elements cannot use depth buffer
    //  - They are supposed to be display from furest to nearest elements
    glDepthMask(false);

    glBindTexture(GL_TEXTURE_2D, reed.texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // avoids sampling artifacts

    for (float i = 0 ; i < reed_positions.size() ; i ++){
        reed.uniform.transform.rotation = scene.camera.orientation;
        reed.uniform.transform.translation = reed_positions[i] + vec3({0.0, 0.0, -0.05});
        draw(reed, scene.camera, shaders["mesh"]);
    }


    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    glDepthMask(true);

    // Display forest
    for (size_t i = 0; i < tree_positions.size(); i++)
    {
        //draw(tree_structures[i%10], scene.camera, branche, feuille,tree_positions[i]);
    }
    // Display big tree
    //draw(grand_arbre, scene.camera, branche, feuille, evaluate_terrain(0.18f, 0.8f,gui_scene)-0.2f);

    

    if( gui_scene.wireframe ){ // wireframe if asked from the GUI
        glPolygonOffset( 1.0, 1.0 );
        vcl::draw(surface, scene.camera, shaders["wireframe"]);
        vcl::draw(cliff, scene.camera, shaders["wireframe"]);
    }

    //Display skybox
    glBindTexture(GL_TEXTURE_2D, sky.texture_id);
    draw(sky, scene.camera, shaders["mesh"]);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);



}


void scene_model::keyboard_input(scene_structure& scene, GLFWwindow* window, int key, int scancode, int action, int mods){
    if ((key == GLFW_KEY_P) & (action == GLFW_PRESS)){ //Changing overview
        if (pedestrian_view){
            last_pedestrian_camera = scene.camera;
            scene.camera = last_overview_camera;
            forward = false;
        }
        if (!(pedestrian_view)){
            last_overview_camera = scene.camera;
            scene.camera = last_pedestrian_camera;
        }
        pedestrian_view = !(pedestrian_view);
    }

    if ((key == GLFW_KEY_F) & (action == GLFW_PRESS) & pedestrian_view){ //Moving forward in pedestrian view
        forward = true;
    }

    if ((key == GLFW_KEY_F) & (action == GLFW_RELEASE) & pedestrian_view){ //Moving forward in pedestrian view
        forward = false;
    }
}


void scene_model::set_gui()
{
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe);
}

#endif

