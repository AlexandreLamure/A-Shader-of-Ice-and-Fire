#include "light.hh"


/* Light ------------------------------------------------------------------------------------------------------------ */

Light::Light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
{
    this->ambient = ambient;
    this->diffuse = diffuse;
    this->specular = specular;
}




/* Directional Light ------------------------------------------------------------------------------------------------ */

DirLight::DirLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 dir)
    : Light(ambient, diffuse, specular)
{
    this->dir = dir;
}

void DirLight::set(Program program, int index)
{
    program.set_vec3("dir_lights[" + std::to_string(index) + "].ambient", ambient);
    program.set_vec3("dir_lights[" + std::to_string(index) + "].diffuse", diffuse);
    program.set_vec3("dir_lights[" + std::to_string(index) + "].specular", specular);
    program.set_vec3("dir_lights[" + std::to_string(index) + "].dir", dir);
}




/* Point Light ------------------------------------------------------------------------------------------------------ */

PointLight::PointLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 pos)
    : Light(ambient, diffuse, specular)
{
    this->pos = pos;
    this->constant = 1.0f;
    this->linear = 0.09f;
    this->quadratic = 0.032f;
}

void PointLight::set(Program program, int index)
{
    program.set_vec3("point_lights[" + std::to_string(index) + "].ambient", ambient);
    program.set_vec3("point_lights[" + std::to_string(index) + "].diffuse", diffuse);
    program.set_vec3("point_lights[" + std::to_string(index) + "].specular", specular);
    program.set_vec3("point_lights[" + std::to_string(index) + "].pos", pos);
    program.set_float("point_lights[" + std::to_string(index) + "].constant", constant);
    program.set_float("point_lights[" + std::to_string(index) + "].linear", linear);
    program.set_float("point_lights[" + std::to_string(index) + "].quadratic", quadratic);
}
