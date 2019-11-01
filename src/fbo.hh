#pragma once

#include <vector>
#include "program.hh"

class FBO
{
private:
    GLuint rbo_id;

public:
    GLuint fbo_id;
    GLuint tex_buffer;

    FBO(int width, int height);
};
