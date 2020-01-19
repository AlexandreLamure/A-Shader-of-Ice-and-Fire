#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex_coords;

out VS_OUT
{
    vec2 tex_coords;
    vec4 color;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 offset;
uniform vec4 color;

void main()
{
    const float scale = 2;
    vs_out.tex_coords = tex_coords;
    vs_out.color = color;
    gl_ClipDistance[0] = 0;
    gl_Position = projection * view * vec4((position * scale) + offset, 1);

    vec3 camera_right = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 camera_up = vec3(view[0][1], view[1][1], view[2][1]);
    gl_Position = projection * view * vec4((offset + (camera_right * position.x * scale) + (camera_up * position.y * scale)), 1);
}