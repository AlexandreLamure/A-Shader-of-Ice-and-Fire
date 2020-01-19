#version 450

in VS_OUT
{
    vec2 tex_coords;
    vec4 color;
} fs_in;

out vec4 output_color;

uniform sampler2D sprite;

void main()
{
    output_color = fs_in.color * vec4(1);//texture(sprite, fs_in.tex_coords)
}