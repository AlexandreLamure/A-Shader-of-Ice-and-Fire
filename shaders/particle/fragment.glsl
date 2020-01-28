#version 450

in GS_OUT
{
    vec2 tex_coords;
} fs_in;

out vec4 output_color;

uniform sampler2D sprite;
uniform vec4 color;

void main()
{
    output_color = color * texture(sprite, fs_in.tex_coords);
}