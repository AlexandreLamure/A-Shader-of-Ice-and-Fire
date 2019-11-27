#pragma once

#include <vector>
#include "program.hh"

class FBO
{
public:
    GLuint fbo_id;
    std::vector<GLuint> color_textures;
    GLuint depth_texture;

    FBO(int width, int height, int nb_data);
};
