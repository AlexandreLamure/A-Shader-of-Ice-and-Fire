#version 450

in GS_OUT
{
    vec2 tex_coords;
    float life;
} fs_in;

out vec4 output_color;

uniform float fade_speed;
uniform sampler2D sprite;
uniform vec4 color;

void main()
{
    output_color = color * texture(sprite, fs_in.tex_coords);
    output_color.a = clamp(fs_in.life * fade_speed, 0, 1);
}