#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex_coords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out VS_OUT
{
    vec4 pos;
    vec3 normal;
    vec2 tex_coords;
    vec3 tangent;
    vec3 bitangent;
} vs_out;



uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int mesh_id;
uniform float total_time;
uniform int rand;

uniform vec4 clip_plane;


void main()
{
    vs_out.pos = vec4(position, 1);
    vs_out.normal = normal;
    vs_out.tex_coords = tex_coords;
    vs_out.tangent = tangent;
    vs_out.bitangent = bitangent;

    gl_Position = vs_out.pos;
}