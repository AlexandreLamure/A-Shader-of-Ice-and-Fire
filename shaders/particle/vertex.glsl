#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex_coords;

out VS_OUT
{
    vec3 pos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;


void main()
{
    gl_ClipDistance[0] = 0;
    vs_out.pos = position;
}