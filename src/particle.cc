#include <iostream>
#include "particle.hh"
#include "model.hh"

Particle::Particle(const glm::vec3& origin)
{
    init(origin);
}

void Particle::init(const glm::vec3& origin)
{
    // positions [-1, 1]
    float random_x = ((std::rand() % 100)) / 100.0f - 0.5f;
    float random_y = ((std::rand() % 100)) / 100.0f - 0.5f;
    float random_z = ((std::rand() % 100)) / 100.0f - 0.5f;
    position = origin + glm::vec3(random_x, random_y, random_z);
    velocity = glm::vec3(0,1,0);
    color = glm::vec4(3.5, 2, 2, 1);
    life = 1.0f;
}

ParticleGenerator::ParticleGenerator(int nb_particles, std::vector<glm::vec3>& origins, const std::string& texture_path)
{
    last_used = 0;
    this->origins = origins;

    for (int i = 0; i < nb_particles; ++i)
        particles.emplace_back(Particle(origins[i]));

    setup_mesh(texture_path);
}

void ParticleGenerator::setup_mesh(const std::string& texture_path)
{
    texture_id = Model::texture_from_file(texture_path.c_str(), ".");

    float particle_quad[] = {
            // positions        tex coords
            -0.5f,  0.5f, 0.f,   0.0f, 1.0f,
             0.5f, -0.5f, 0.f,   1.0f, 0.0f,
            -0.5f, -0.5f, 0.f,   0.0f, 0.0f,

            -0.5f,  0.5f, 0.f,   0.0f, 1.0f,
             0.5f,  0.5f, 0.f,   1.0f, 1.0f,
             0.5f, -0.5f, 0.f,   1.0f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // vertex texture coords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
}

int ParticleGenerator::get_first_dead()
{
    // Search from last used particle
    for (int i = last_used; i < particles.size(); ++i){
        if (particles[i].life <= 0)
        {
            last_used = i;
            return i;
        }
    }
    // Otherwise, do a linear search
    for (int i = 0; i < last_used; ++i)
    {
        if (particles[i].life <= 0)
        {
            last_used = i;
            return i;
        }
    }
    return -1;
}

void ParticleGenerator::update(float delta_time, float total_time)
{
    // Reset dead particles
    int dead_id = get_first_dead();
    if (dead_id != -1)
        particles[dead_id].init(origins[dead_id]);

    // Update values
    for (int i = 0; i < particles.size(); ++i)
    {
        Particle& p = particles[i];
        p.life -= delta_time * 0.8; // reduce life
        if (p.life > 0)
        {
            p.velocity.x = cos(total_time / 2 + i / 3.14) * 0.6;
            p.velocity.z = sin(total_time / 2 + i / 2) * 0.6;
            p.position += p.velocity * delta_time;
            p.color.a = p.life * 1.5;
            if (p.color.a < 0)
                p.color.a = 0;
        }
    }
}

void ParticleGenerator::draw(Program& program)
{
    for (Particle& p : particles)
    {
        if (p.life > 0.0f)
        {
            program.set_vec3("offset", p.position);
            program.set_vec4("color", p.color);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id);
            program.set_int("sprite", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
}

LavaParticleGenerator::LavaParticleGenerator(int nb_particles, std::vector<glm::vec3>& origins, const std::string& texture_path)
    : ParticleGenerator(nb_particles, origins, texture_path)
{}

LavaParticleGenerator init_lava_particle_generator(const Model& lava)
{
    std::vector<glm::vec3> origins;
    for (const Mesh& mesh : lava.meshes)
    {
        for (int i = 0; i < mesh.vertices.size(); i+=15)
            origins.emplace_back(mesh.vertices[i].position);
    }
    std::cout << origins.size() << " origins" << std::endl;
    return LavaParticleGenerator(origins.size(), origins, "../models/particle/lava.png");
}