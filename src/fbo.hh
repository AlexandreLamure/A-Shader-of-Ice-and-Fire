#pragma once

#include <vector>
#include "program.hh"

class FBO
{
private:
    void init(int width, int height);

public:
    GLuint fbo_id;
    std::vector<GLuint> color_textures;
    GLuint depth_texture;

    FBO(int width, int height, int nb_data);
    void realloc(int width, int height);
};
