#version 450

layout (location = 0) in vec4 position; // Computer shader need to use vec4
layout (location = 1) in float life;

out VS_OUT
{
    vec3 pos;
    float life;
} vs_out;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_ClipDistance[0] = 0;
    vs_out.pos = position.xyz;
    vs_out.life = life;

    gl_Position = projection * view * vec4(position.xyz, 1);
}