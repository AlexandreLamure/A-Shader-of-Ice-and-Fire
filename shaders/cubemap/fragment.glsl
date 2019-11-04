#version 450

in vec3 tex_coords;

out vec4 output_color;

uniform samplerCube texture_cubemap;


void main()
{
    output_color = texture(texture_cubemap, tex_coords);
}