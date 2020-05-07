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

}



/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& ){

    set_gui();

    glEnable( GL_POLYGON_OFFSET_FILL ); // avoids z-fighting when displaying wireframe


    // Display terrain
    glPolygonOffset( 1.0, 1.0 );
    vcl::draw(surface, scene.camera, shaders["mesh"]);

    if( gui_scene.wireframe ){ // wireframe if asked from the GUI
        glPolygonOffset( 1.0, 1.0 );
        vcl::draw(surface, scene.camera, shaders["wireframe"]);
    }

}

void scene_model::set_gui()
{
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe);
}

#endif

