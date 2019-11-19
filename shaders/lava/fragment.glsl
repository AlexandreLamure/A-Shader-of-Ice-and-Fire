#version 450

in GS_OUT
{
    vec4 pos;
    vec3 normal;
    vec2 tex_coords;
    vec4 clip_space;
} fs_in;

out vec4 output_color;

#define PI 3.141592

uniform float total_time;
uniform vec3 camera_pos;
uniform float wave_speed;

// from lava_texture.glsl
vec4 lava_texture_mix(vec3 position, float total_time);

// from ice.glsl
float get_ice_state(vec4 position, float total_time, float wave_speed);
float get_ice_wave(float ice_state);


void main()
{
    output_color = vec4(1);

    /* ------------------------------------------------------- */
    /* ------------------------------------------------------- */

    vec3 normal = fs_in.normal; // if no normal map

    /* ------------------------------------------------------- */
    /* ------------------------------------------------------- */

    output_color = lava_texture_mix(fs_in.pos.xyz, total_time);

    // Ice color
    const float transition_speed = 4;
    output_color = mix(output_color, vec4(0, 0, 0, 1), clamp(get_ice_state(fs_in.pos, total_time, wave_speed) * transition_speed, 0, 1));
}