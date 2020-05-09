#include "project.hpp"
#include "global_setting_modeling_functions.hpp"
#include "plants_modeling_functions.hpp"
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
    cliff.uniform.color = {0.8f,0.0f,0.0f};
    cliff.texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/05_project/assets/cliff.png"));

    //Create moss
    moss = create_moss();
    moss.texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/05_project/assets/moss.png"));
    update_position(500,moss_positions,0, gui_scene);

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

    for (vec3 position : moss_positions){
        moss.uniform.transform.translation = position;
        draw(moss, scene.camera, shaders["mesh"]);
    }

    glBindTexture(GL_TEXTURE_2D, scene.texture_white);



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

