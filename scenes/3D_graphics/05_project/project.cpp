#include "project.hpp"
#include "global_setting_modeling_functions.hpp"
#include "plants_modeling_functions.hpp"
#include "human_activity_modeling_functions.hpp"

using namespace vcl;

#ifdef PROJECT

/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_model::setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& ){

    // Create visual terrain surface
    surface = create_terrain(gui_scene);
    surface.uniform.shading.specular = 0.0f; // non-specular terrain material
    surface.texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/05_project/assets/forest_floor_2.png"));


    // Setup initial camera mode and position
    scene.camera.camera_type = vcl::camera_control_spherical_coordinates;
    scene.camera.scale = 20.0f;
    scene.camera.apply_rotation(0,0,0,1.2f);

    //Create cliff
    cliff = create_cliff();
    cliff.uniform.color = {1.0f,1.0f,1.0f};
    cliff.uniform.shading.ambiant = 0.8f;
    cliff.texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/05_project/assets/cliff_4.png"));

    //Create water
    water = create_water(gui_scene);
    water.uniform.color = {0.2f, 0.6, 1.0};

    //Create moss
    moss = create_moss();
    moss.uniform.shading.specular = 0.0f; // non-specular terrain material
    moss.texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/05_project/assets/moss.png"));
    update_position_forest(500,moss_positions,0, gui_scene);
    update_size(500, moss_sizes);

    //Create wall
    wall_size = 1.0;
    wall = create_wall(wall_size);
    wall_texture = create_texture_gpu(image_load_png("scenes/3D_graphics/05_project/assets/cabane_mur_simple.png"));
    wall_window_texture = create_texture_gpu(image_load_png("scenes/3D_graphics/05_project/assets/cabane_fenetre.png"));
    wall_window_door_texture = create_texture_gpu(image_load_png("scenes/3D_graphics/05_project/assets/cabane_porte_fenetre.png"));
    roof_texture = create_texture_gpu(image_load_png("scenes/3D_graphics/05_project/assets/toit.png"));

    //Create reed
    reed = create_reed();
    reed.uniform.shading = {1,0,0}; // set pure ambiant component (no diffuse, no specular) - allow to only see the color of the texture
    reed.uniform.transform.scaling = 0.5;
    reed.texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/05_project/assets/billboard_reed_2.png"), GL_REPEAT, GL_REPEAT);
    update_position_river(1500, reed_positions, 0, gui_scene);

    //Create visual elements for the trees
    float taille_branche = 0.3f;
    branche = vcl::branche(taille_branche);
    feuille = vcl::feuille(taille_branche);
    branche.shader = shaders["mesh"]; feuille.shader = shaders["mesh"];
    update_position_forest(25, tree_positions ,10*taille_branche, gui_scene);
    //Structure des arbres
    for (int i = 0; i < 10; i++)
    {
        noeud* arbre = grammar_tree(4);
        fill_coordinates(arbre, taille_branche);
        tree_structures.push_back(arbre);
    }
    // un arbre plus grand à côté de la maison
    grand_arbre = grammar_tree(5);
    fill_coordinates(grand_arbre, taille_branche);
    
}



/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& ){

    set_gui();

    glEnable( GL_POLYGON_OFFSET_FILL ); // avoids z-fighting when displaying wireframe


    // Display terrain
    glPolygonOffset( 1.0, 1.0 );
    glBindTexture(GL_TEXTURE_2D, surface.texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    draw(surface, scene.camera, shaders["mesh"]);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);


    // Display water
    draw(water, scene.camera, shaders["mesh"]);

    // Display cliff
    glBindTexture(GL_TEXTURE_2D, cliff.texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    cliff.uniform.transform.scaling = 30;
    cliff.uniform.transform.translation = vec3(10, 10, -1);
    draw(cliff, scene.camera, shaders["mesh"]);
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
    wall.uniform.transform.translation = evaluate_terrain(0.2, 0.8, gui_scene);
    draw(wall, scene.camera, shaders["mesh"]);

    glBindTexture(GL_TEXTURE_2D, wall_window_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    angle = angle - 3.14/2;
    wall.uniform.transform.rotation = {{std::cos(angle), -std::sin(angle), 0.0}, {std::sin(angle), std::cos(angle), 0.0},{0.0, 0.0, 1.0}};
    wall.uniform.transform.translation = evaluate_terrain(0.2, 0.8, gui_scene);
    draw(wall, scene.camera, shaders["mesh"]);

    glBindTexture(GL_TEXTURE_2D, wall_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    wall.uniform.transform.rotation = {{std::cos(angle), -std::sin(angle), 0.0}, {std::sin(angle), std::cos(angle), 0.0},{0.0, 0.0, 1.0}};
    wall.uniform.transform.translation = evaluate_terrain(0.2f, 0.8f, gui_scene) + vec3({std::cos(3.14f/6)*wall_size, std::sin(3.14f/6)*wall_size, 0});
    draw(wall, scene.camera, shaders["mesh"]);

    angle = angle + 3.14/2;
    wall.uniform.transform.rotation = {{std::cos(angle), -std::sin(angle), 0.0}, {std::sin(angle), std::cos(angle), 0.0},{0.0, 0.0, 1.0}};
    wall.uniform.transform.translation = evaluate_terrain(0.2f, 0.8f, gui_scene) + vec3({-std::cos(3.14f/3)*wall_size, std::sin(3.14f/3)*wall_size, 0});
    draw(wall, scene.camera, shaders["mesh"]);

    glBindTexture(GL_TEXTURE_2D, roof_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    wall.uniform.transform.rotation = {{std::cos(angle), -std::sin(angle), 0.0}, {0, 0, 1.0},{std::sin(angle),std::cos(angle), 0}};
    wall.uniform.transform.translation = evaluate_terrain(0.2, 0.8, gui_scene) + vec3({0.0, 0.0,wall_size});
    draw(wall, scene.camera, shaders["mesh"]);


    glBindTexture(GL_TEXTURE_2D, scene.texture_white);


    // Diplay reed (LAST DISPLAY)

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
        draw(tree_structures[i%10], scene.camera, branche, feuille,tree_positions[i]);
    }
    // Display tree
    draw(grand_arbre, scene.camera, branche, feuille, evaluate_terrain(0.1f, 0.8f,gui_scene));


    if( gui_scene.wireframe ){ // wireframe if asked from the GUI
        glPolygonOffset( 1.0, 1.0 );
        vcl::draw(surface, scene.camera, shaders["wireframe"]);
        vcl::draw(cliff, scene.camera, shaders["wireframe"]);
    }

}

void scene_model::set_gui()
{
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe);
}

#endif

