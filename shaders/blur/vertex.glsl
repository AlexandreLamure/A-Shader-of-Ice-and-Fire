#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT
{
    vec2 tex_coords;
} vs_out;


void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    vs_out.tex_coords = aTexCoords;
}