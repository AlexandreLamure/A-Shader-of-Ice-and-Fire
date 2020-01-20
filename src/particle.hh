#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "program.hh"
#include "mesh.hh"
#include "model.hh"

class Particle {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;
    float scale;
    float life;

    explicit Particle(const glm::vec3& origin, glm::vec3 randomness, glm::vec3 velocity, glm::vec4 color, float scale);
    void init(const glm::vec3& origin, glm::vec3 randomness, glm::vec3 velocity, glm::vec4 color, float scale);
};


class ParticleGenerator {
protected:
    std::vector<Particle> particles;
    unsigned int texture_id;
    unsigned int VAO, VBO;
    int last_used;
    std::vector<glm::vec3> origins;

    int get_first_dead();

public:
    ParticleGenerator(std::vector<glm::vec3>& origins, const std::string& texture_path);

    virtual void update(float delta_time, float total_time) = 0;
    void draw(Program& program);
};


class LavaParticleGenerator : public ParticleGenerator {
public:
    LavaParticleGenerator(std::vector<glm::vec3>& origins, const std::string& texture_path);
    void update(float delta_time, float total_time);
};

LavaParticleGenerator init_lava_particle_generator(const Model& lava);


class SnowParticleGenerator : public ParticleGenerator {
public:
    SnowParticleGenerator(std::vector<glm::vec3>& origins, const std::string& texture_path);
    void update(float delta_time, float total_time);
};

SnowParticleGenerator init_snow_particle_generator();
