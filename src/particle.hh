#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "program.hh"
#include "mesh.hh"

class Particle {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;
    float life;

    explicit Particle(const glm::vec3& origin);
    void init(const glm::vec3& origin);
};

class ParticleGenerator {
private:
    std::vector<Particle> particles;
    unsigned int texture_id;
    unsigned int VAO, VBO;
    int last_used;

    void setup_mesh(const std::string& texture_path);
    int get_first_dead();

public:
    ParticleGenerator(int nb_particles, const std::string& texture_path);
    void update(float delta_time, int nb_new);
    void draw(Program& program);
};