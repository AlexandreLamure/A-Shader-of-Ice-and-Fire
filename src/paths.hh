#pragma once

#include <vector>

namespace PATHS
{
    // VOLCANO PROGRAM -------------------------------------------------------------------------------------------------
    const std::vector<const char*> volcano_vertex{"../shaders/volcano/vertex.glsl"};
    const std::vector<const char*> volcano_tc{};
    const std::vector<const char*> volcano_te{};
    const std::vector<const char*> volcano_geometry{"../shaders/volcano/geometry.glsl"};
    const std::vector<const char*> volcano_frag{"../shaders/utils/ice.glsl",
                                                "../shaders/utils/simplex.glsl",
                                                "../shaders/volcano/fragment.glsl"};


    // LAVA PROGRAM ----------------------------------------------------------------------------------------------------
    const std::vector<const char*> lava_vertex{"../shaders/lava/vertex.glsl"};
    const std::vector<const char*> lava_tc{"../shaders/lava/tcontrol.glsl"};
    const std::vector<const char*> lava_te{"../shaders/lava/teval.glsl"};
    const std::vector<const char*> lava_geometry{"../shaders/utils/misc.glsl",
                                                 "../shaders/utils/simplex.glsl",
                                                 "../shaders/utils/lava_texture.glsl",
                                                 "../shaders/utils/ice.glsl",
                                                 "../shaders/lava/geometry.glsl"};
    const std::vector<const char*> lava_frag{"../shaders/utils/misc.glsl",
                                             "../shaders/utils/simplex.glsl",
                                             "../shaders/utils/lava_texture.glsl",
                                             "../shaders/utils/ice.glsl",
                                             "../shaders/lava/fragment.glsl"};


    // WATER PROGRAM ---------------------------------------------------------------------------------------------------
    const std::vector<const char*> water_vertex{"../shaders/water/vertex.glsl"};
    const std::vector<const char*> water_tc{"../shaders/water/tcontrol.glsl"};
    const std::vector<const char*> water_te{"../shaders/water/teval.glsl"};
    const std::vector<const char*> water_geometry{"../shaders/utils/ice.glsl",
                                                  "../shaders/water/geometry.glsl"};
    const std::vector<const char*> water_frag{"../shaders/utils/ice.glsl",
                                              "../shaders/water/fragment.glsl"};


    // CUBEMAP PROGRAM -------------------------------------------------------------------------------------------------
    const std::vector<const char*> cubemap_vertex{"../shaders/cubemap/vertex.glsl"};
    const std::vector<const char*> cubemap_tc{};
    const std::vector<const char*> cubemap_te{};
    const std::vector<const char*> cubemap_geometry{};
    const std::vector<const char*> cubemap_frag{"../shaders/cubemap/fragment.glsl"};


    // LIGHT PROGRAM --------------------------------------------------------------------------------------------------
    const std::vector<const char*> light_vertex{"../shaders/light/vertex.glsl"};
    const std::vector<const char*> light_tc{};
    const std::vector<const char*> light_te{};
    const std::vector<const char*> light_geometry{};
    const std::vector<const char*> light_frag{"../shaders/utils/ice.glsl", "../shaders/light/fragment.glsl"};


    // BLOOM PROGRAM --------------------------------------------------------------------------------------------------
    const std::vector<const char*> bloom_vertex{"../shaders/bloom/vertex.glsl"};
    const std::vector<const char*> bloom_tc{};
    const std::vector<const char*> bloom_te{};
    const std::vector<const char*> bloom_geometry{};
    const std::vector<const char*> bloom_frag{"../shaders/bloom/fragment.glsl"};


    // BLUR PROGRAM --------------------------------------------------------------------------------------------------
    const std::vector<const char*> blur_vertex{"../shaders/blur/vertex.glsl"};
    const std::vector<const char*> blur_tc{};
    const std::vector<const char*> blur_te{};
    const std::vector<const char*> blur_geometry{};
    const std::vector<const char*> blur_frag{"../shaders/blur/fragment.glsl"};


    // SCREEN PROGRAM --------------------------------------------------------------------------------------------------
    const std::vector<const char*> screen_vertex{"../shaders/screen/vertex.glsl"};
    const std::vector<const char*> screen_tc{};
    const std::vector<const char*> screen_te{};
    const std::vector<const char*> screen_geometry{};
    const std::vector<const char*> screen_frag{"../shaders/screen/fragment.glsl"};

}