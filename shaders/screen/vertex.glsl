#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex_coords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out VS_OUT
{
    vec2 tex_coords;
} vs_out;


void main()
{
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    vs_out.tex_coords = tex_coords;
}