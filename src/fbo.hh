#pragma once

#include <vector>
#include "program.hh"

class FBO
{
public:
    GLuint fbo_id;
    GLuint color_texture;
    GLuint depth_texture;

    FBO(int width, int height);
};
