#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform float wave_speed;
uniform float total_time;
uniform int mesh_id;

in TES_OUT
{
    vec4 pos;
    vec3 normal;
    vec2 tex_coords;
    vec4 clip_space;
} gs_in[];

out GS_OUT
{
    vec4 pos;
    vec3 normal;
    vec2 tex_coords;
    vec4 clip_space;
} gs_out;

vec4 lava_texture_real(vec3 position, float total_time);

void set_out(int index)
{
    gs_out.pos = gs_in[index].pos;
    gs_out.normal = gs_in[index].normal;
    gs_out.tex_coords = gs_in[index].tex_coords;
    gs_out.clip_space = gs_in[index].clip_space;
    gl_Position = gl_in[index].gl_Position;
    gl_ClipDistance[0] = gl_in[index].gl_ClipDistance[0];

    vec4 tex = lava_texture_real(gs_in[index].pos.xyz, total_time);
    float d = (tex.r + tex.g + tex.b) * 0.15;
    gl_Position.y += d;
    gs_out.pos.y += d;
}

void main()
{
    set_out(0);
    EmitVertex();

    set_out(1);
    EmitVertex();

    set_out(2);
    EmitVertex();

    EndPrimitive();
}