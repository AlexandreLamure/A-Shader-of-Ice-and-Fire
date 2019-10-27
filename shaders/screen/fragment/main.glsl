#version 450

in VS_OUT
{
    vec2 tex_coords;
} fs_in;

out vec4 output_color;


uniform sampler2D screen_texture;

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

    output_color *= texture(screen_texture, fs_in.tex_coords);
}