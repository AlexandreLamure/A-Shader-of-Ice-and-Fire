#include <iostream>
#include "particle.hh"
#include "model.hh"


ParticleGenerator::ParticleGenerator(std::vector<glm::vec4>& origins, const std::string& texture_path,
                                     const glm::vec3& randomness, const glm::vec3& velocity, const glm::vec4& color,
                                     float scale, float death_speed, float fade_speed, float spin_range)
{
    this->origins = origins;
    this->randomness = randomness;
    this->velocity = velocity;
    this->color = color;
    this->scale = scale;
    this->death_speed = death_speed;
    this->fade_speed = fade_speed;
    this->spin_range = spin_range;

    texture_id = Model::texture_from_file(texture_path.c_str(), ".");

    std::vector<glm::vec4> positions = std::vector<glm::vec4>(origins.size());
    std::vector<glm::vec4> velocities = std::vector<glm::vec4>(origins.size());
    std::vector<float> lives = std::vector<float>(origins.size());
    for (int i = 0; i < origins.size(); ++i)
    {
        // positions [-1, 1] * randomness
        float random_x = ((std::rand() % 100) / 50.f - 1.f) * randomness.x;
        float random_y = ((std::rand() % 100) / 50.f - 1.f) * randomness.y;
        float random_z = ((std::rand() % 100) / 50.f - 1.f) * randomness.z;

        positions[i] = origins[i] + glm::vec4(random_x, random_y, random_z, 0);
        velocities[i] = glm::vec4(velocity, -1);
        lives[i] = 1.0f + ((std::rand() % 100) / 100.f - 0.5f);
    }

    glGenBuffers(1, &origin_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, origin_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, origins.size() * sizeof(glm::vec4), &(origins[0]), GL_STATIC_COPY);

    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, position_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, origins.size() * sizeof(glm::vec4), &(positions[0]), GL_DYNAMIC_COPY);

    glGenBuffers(1, &velocity_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocity_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, origins.size() * sizeof(glm::vec4), &(velocities[0]), GL_DYNAMIC_COPY);

    glGenBuffers(1, &life_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, life_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, origins.size() * sizeof(float), &(lives[0]), GL_DYNAMIC_COPY);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


    // Define VAO to allow drawing
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, life_buffer);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);
}

void ParticleGenerator::compute(Program program, float delta_time, float total_time)
{
    program.set_float("delta_time", delta_time);
    program.set_float("total_time", total_time);
    program.set_vec3("randomness", randomness);
    program.set_vec3("start_velocity", velocity);
    program.set_float("death_speed", death_speed);
    program.set_float("spin_range", spin_range);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, origin_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, position_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, velocity_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, life_buffer);

    glDispatchCompute(std::ceil(origins.size() / 128.f), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void ParticleGenerator::draw(Program& program)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(VAO);

    program.set_vec4("color", color);
    program.set_float("scale", scale);
    program.set_float("fade_speed", fade_speed);
    program.set_int("sprite", 0);

    // Send 'nb_origins' points. The geometry shader change the draw mode to triangle_strip.
    // For example, 100 particles will send 100 GL_POINTS and draw 600 vertices (two triangles per particle).
    glDrawArrays(GL_POINTS, 0, origins.size());
}

ParticleGenerator init_lava_particle_generator(const Model& lava)
{
    std::vector<glm::vec4> origins;
    for (const Mesh& mesh : lava.meshes)
    {
        for (int i = 0; i < mesh.vertices.size(); i+=24)
            origins.emplace_back(glm::vec4(mesh.vertices[i].position, -1));
    }
    std::cout << origins.size() << " lava origins" << std::endl;

    return ParticleGenerator(origins, "../models/particle/lava.png",
                             glm::vec3(0.8),           // randomness
                             glm::vec3(0, 0.2, 0),     // velocity
                             glm::vec4(6, 3, 3, 1),    // color
                             0.15,                     // scale
                             0.2,                      // death_speed
                             1.5,                      // fade_speed
                             0.2);                     // spin_range
}

ParticleGenerator init_snow_particle_generator()
{
    std::vector<glm::vec4> origins;
    for (int i = -50; i < 50; i += 3)
    {
        for (int j = -50; j < 50; j += 3)
            origins.emplace_back(glm::vec4(i, 70, j, -1));
    }
    std::cout << origins.size() << " snow origins" << std::endl;

    return ParticleGenerator(origins, "../models/particle/snow.png",
                             glm::vec3(10,25,10),      // randomness
                             glm::vec3(0, -2, 0),      // velocity
                             glm::vec4(2, 2, 2, 1),    // color
                             0.2,                     // scale
                             0.03,                      // death_speed
                             2,                      // fade_speed
                             0.6);                     // spin_range
}