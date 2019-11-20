#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

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


uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;

uniform float total_time;


// from lava_texture.glsl
vec4 lava_texture_real(vec3 position);

// from ice.glsl
float get_ice_state(vec4 position);
float get_ice_wave(float ice_state);


void set_out(int index)
{
    gs_out.pos = gs_in[index].pos;
    gs_out.normal = gs_in[index].normal;
    gs_out.tex_coords = gs_in[index].tex_coords;
    gs_out.clip_space = gs_in[index].clip_space;
    gl_Position = gl_in[index].gl_Position;
    gl_ClipDistance[0] = gl_in[index].gl_ClipDistance[0];

    // Compute decay
    vec4 tex = lava_texture_real(gs_in[index].pos.xyz);
    float d = (tex.r + tex.g + tex.b) * 0.15;
    // Ice transition
    const float transition_speed = 4;
    d = mix(d, 0, clamp(get_ice_state(gs_in[index].pos) * transition_speed, 0, 1));
    // Apply decay
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