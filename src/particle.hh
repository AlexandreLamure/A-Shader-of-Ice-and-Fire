#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "program.hh"
#include "mesh.hh"
#include "model.hh"


class ParticleGenerator {
private:
    GLuint VAO;
    GLuint origin_buffer;
    GLuint position_buffer;
    GLuint velocity_buffer;
    GLuint life_buffer;

    unsigned int texture_id;
    std::vector<glm::vec4> origins;

    glm::vec3 randomness;
    glm::vec3 velocity;
    glm::vec4 color;
    float scale;
    float death_speed;
    float fade_speed;
    float spin_radius;
    float spin_height;

public:
    ParticleGenerator(std::vector<glm::vec4>& origins, const std::string& texture_path,
                      const glm::vec3& randomness, const glm::vec3& velocity, const glm::vec4& color,
                      float scale, float death_speed, float fade_speed, float spin_radius, float spin_height);

    void compute(Program program, float delta_time, float total_time);
    void draw(Program& program);
};

ParticleGenerator init_lava_particle_generator(const Model& lava);
ParticleGenerator init_snow_particle_generator();
ParticleGenerator init_smoke_particle_generator(std::vector<glm::vec3> lava_nexus);
ParticleGenerator init_moth_particle_generator(const Model& water);
ParticleGenerator init_light_moth_particle_generator(const std::vector<LightModel>& light_models);
