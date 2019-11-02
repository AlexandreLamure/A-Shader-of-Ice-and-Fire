#version 450

in VS_OUT
{
    vec2 tex_coords;
} fs_in;

out vec4 output_color;


uniform sampler2D texture_other0;

uniform float total_time;
uniform vec2 resolution;

uniform vec3 camera_pos;
uniform int mesh_id;
uniform int rand;


void main()
{
    output_color = vec4(1);

    /* ------------------------------------------------------- */
    /* ------------------------------------------------------- */

    output_color *= texture(texture_other0, fs_in.tex_coords);
}