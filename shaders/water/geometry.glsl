#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT
{
    vec4 pos;
    vec3 normal;
    vec2 tex_coords;
    mat3 TBN;
    vec4 clip_space;
} gs_in[];



out GS_OUT
{
    vec4 pos;
    vec3 normal;
    vec2 tex_coords;
    mat3 TBN;
    vec4 clip_space;
} gs_out;


void set_out(int index)
{
    gs_out.pos = gs_in[index].pos;
    gs_out.normal = gs_in[index].normal;
    gs_out.tex_coords = gs_in[index].tex_coords;
    gs_out.TBN = gs_in[index].TBN;
    gs_out.clip_space = gs_in[index].clip_space;
    gl_Position = gl_in[index].gl_Position;
    gl_ClipDistance[0] = gl_in[index].gl_ClipDistance[0];
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