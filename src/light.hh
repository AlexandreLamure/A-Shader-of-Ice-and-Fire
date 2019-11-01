#pragma once

#include <glm/glm.hpp>
#include "program.hh"

class Light
{
public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    Light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
    virtual void set(Program program, int index) = 0;
};

class DirLight: public Light
{
public:
    glm::vec3 dir;

    DirLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 dir);
    void set(Program program, int index);
};


class PointLight: public Light
{
public:
    glm::vec3 pos;
    float constant;
    float linear;
    float quadratic;

    PointLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 pos);
    void set(Program program, int index);
};