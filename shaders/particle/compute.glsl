#version 450

// Process particles in blocks of 128
layout (local_size_x = 128, local_size_y = 1, local_size_z = 1) in;

// FIXME: Computer shader seems to need to use vec4 instead of vec3. origins, positions and velocities should be vec3
layout (std430, binding = 0) buffer origin_buffer
{
    vec4 origins[];
};

layout (std430, binding = 1) buffer position_buffer
{
    vec4 positions[];
};

layout (std430, binding = 2) buffer velocity_buffer
{
    vec4 velocities[];
};

layout (std430, binding = 3) buffer life_buffer
{
    float lives[];
};


uniform float delta_time;
uniform float total_time;
uniform vec3 randomness;
uniform vec3 start_velocity;
uniform float death_speed;
uniform float spin_radius;
uniform float spin_height;

uniform float water_h;

float random(vec2 st);

void main(void)
{
    uint index = gl_GlobalInvocationID.x;
    vec4 origin = origins[index];
    vec4 pos = positions[index];
    vec4 vel = velocities[index];
    float life = lives[index];

    life -= delta_time * death_speed;

    vel.x = cos(total_time / spin_height + index / 3.14) * spin_radius;
    vel.z = sin(total_time / spin_height + index / 2) * spin_radius;
    pos += vel * delta_time;

    // Remove particles under water
    if (pos.y < water_h) // FIXME to be smooth
        life -= 0.2;

    // Reset particle
    if (life < 0)
    {
        // positions [-1, 1] * randomness
        float random_x = (random(origin.xz * total_time + index) * 2.f - 1.f) * randomness.x;
        float random_y = (random(origin.yz * total_time + index) * 2.f - 1.f) * randomness.y;
        float random_z = (random(origin.xz * total_time + index) * 2.f - 1.f) * randomness.z;
        pos = origin + vec4(random_x, random_y, random_z, 0);
        //vel.xyz = start_velocity;
        life = 1.0f;
    }

    positions[index] = pos;
    //velocities[index] = vel; // useless because we never add or multiply
    lives[index] = life;
}