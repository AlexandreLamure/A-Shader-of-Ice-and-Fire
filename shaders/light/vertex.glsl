#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex_coords;

out VS_OUT
{
    vec4 pos;
    vec3 normal;
    vec2 tex_coords;
} vs_out;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec4 clip_plane;

void main()
{
    vs_out.pos = model * vec4(position, 1);
    vs_out.normal = mat3(transpose(inverse(model))) * normal; // we only keep the scale and rotations from model matrix
    vs_out.tex_coords = tex_coords;

    gl_ClipDistance[0] = dot(vs_out.pos, clip_plane);
    gl_Position = projection * view * vs_out.pos;
}