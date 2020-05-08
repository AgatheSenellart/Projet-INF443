#include "project.hpp"
#include "global_setting_modeling_functions.hpp"
#include "plants_modeling_functions.hpp"
#ifdef PROJECT


/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_model::setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& ){

    // Create visual terrain surface
    surface = create_terrain(gui_scene);
    surface.uniform.color = {0.6f,0.85f,0.5f};
    surface.uniform.shading.specular = 0.0f; // non-specular terrain material

    // Setup initial camera mode and position
    scene.camera.camera_type = vcl::camera_control_spherical_coordinates;
    scene.camera.scale = 20.0f;
    scene.camera.apply_rotation(0,0,0,1.2f);

    //Create cliff
    cliff = create_cliff();
    cliff.uniform.color = {0.8f,0.0f,0.0f};

    /*
          // Create tree
    tree_cylinder = create_cylinder(0.15, 0.8);
    tree_cylinder.uniform.color = {0.5f,0.7f,0.5f};
    tree_cone = create_tree_foliage(0.5, 0.6, 0.4);
    tree_cone.uniform.color = {0.0f,0.85f,0.0f};

    update_position(60,tree_positions,0.5, gui_scene);

     // Create mushroom
     mushroom_cylinder = create_cylinder(0.05, 0.2);
     mushroom_cylinder.uniform.color = {0.0f,0.0f,0.0f};
     mushroom_cone = create_cone(0.2, 0.2, 0.2);
     mushroom_cone.uniform.color = {0.80f,0.0f,0.0f};

     update_position(80,mushroom_positions,0.3, gui_scene);
     */
}



/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& ){

    set_gui();

    glEnable( GL_POLYGON_OFFSET_FILL ); // avoids z-fighting when displaying wireframe


    // Display terrain
    glPolygonOffset( 1.0, 1.0 );
    draw(surface, scene.camera, shaders["mesh"]);

    // Display cliff
    cliff.uniform.transform.scaling = 30;
    cliff.uniform.transform.translation = vec3(10, 10, -1);
    draw(cliff, scene.camera, shaders["mesh"]);


 /*   // Display trees
    for (vec3 position : tree_positions){
        tree_cone.uniform.transform.translation = position + vec3(0.0f, 0.0f, 0.8f)+ vec3(0.0f, 0.0f, -0.2f);
        tree_cylinder.uniform.transform.translation = position + vec3(0.0f, 0.0f, -0.2f);
        draw(tree_cone, scene.camera, shaders["mesh"]);
        draw(tree_cylinder, scene.camera, shaders["mesh"]);
    }

    // Display mushrooms
    for (vec3 position : mushroom_positions){
        mushroom_cone.uniform.transform.translation = position + vec3(0.0f, 0.0f, -0.02f);
        mushroom_cylinder.uniform.transform.translation = position + vec3(0.0f, 0.0f, -0.02f);
        draw(mushroom_cone, scene.camera, shaders["mesh"]);
        draw(mushroom_cylinder, scene.camera, shaders["mesh"]);
    }*/

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

