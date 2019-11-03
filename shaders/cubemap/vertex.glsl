#version 450

layout (location = 0) in vec3 position;

out vec3 tex_coords;

uniform mat4 view;
uniform mat4 projection;


void main()
{
    tex_coords = position;
    vec4 pos = projection * view * vec4(position, 1.0);
    gl_Position = pos.xyww;
}