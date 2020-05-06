
#include "project.hpp"
#include "modeling_functions.hpp"

#ifdef PROJECT


/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_model::setup_data(std::map<std::string,GLuint>& shaders, scene_structure& , gui_structure& ){

    // Create visual terrain surface
    surface = create_terrain();
    surface.uniform.shading.specular = 0.0f; // non-specular terrain material



}




/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& ){

    draw(surface, scene.camera, shaders["mesh"]);

}


#endif

