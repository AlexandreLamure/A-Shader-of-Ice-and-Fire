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


vec4 lava_texture_mix(vec3 position, float total_time);


void main()
{
    output_color = vec4(1);

    /* ------------------------------------------------------- */
    /* ------------------------------------------------------- */

    vec3 normal = fs_in.normal; // if no normal map

    /* ------------------------------------------------------- */
    /* ------------------------------------------------------- */

    output_color = lava_texture_mix(fs_in.pos.xyz, total_time);
}