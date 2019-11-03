#pragma once


#include "program.hh"

class Cubemap
{
private:
    GLuint VAO;
    GLuint texture;

    void load_textures();


public:
    Cubemap();
    void draw(Program program);
};