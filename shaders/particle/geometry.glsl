#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices = 4) out;

in VS_OUT
{
    vec3 pos;
} gs_in[];

out GS_OUT
{
    vec2 tex_coords;
} gs_out;


uniform mat4 projection;
uniform mat4 view;

uniform vec3 offset;
uniform float scale;


void set_out(vec2 quad_pos, vec2 tex_coords)
{
    gs_out.tex_coords = tex_coords;

    // always face the camera
    vec3 camera_right = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 camera_up = vec3(view[0][1], view[1][1], view[2][1]);
    gl_Position = projection * view * vec4((offset + (camera_right * quad_pos.x * scale) + (camera_up * quad_pos.y * scale)), 1);
}

void main()
{
    // Draws 2 triangles

    // Left down
    set_out(vec2(-0.5f,  -0.5f), vec2(0.0f, 0.0f));
    EmitVertex();

    // Left up
    set_out(vec2(-0.5f,  0.5f), vec2(0.0f, 1.0f));
    EmitVertex();

    // Right down
    set_out(vec2(0.5f,  -0.5f), vec2(1.0f, 0.0f));
    EmitVertex();

    // Right up
    set_out(vec2(0.5f,  0.5f), vec2(1.0f, 1.0f));
    EmitVertex();

    EndPrimitive();
}