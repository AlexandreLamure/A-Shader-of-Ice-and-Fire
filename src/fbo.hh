#pragma once

#include <vector>
#include "program.hh"

class FBO
{
public:
    GLuint fbo_id;
    GLuint tex_buffer;
    GLuint rbo_id;

    FBO(int width, int height);
};
