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
    mat3 TBN;
} vs_out;



uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float total_time;

uniform vec4 clip_plane;


void main()
{
    vs_out.pos = model * vec4(position, 1);
    vs_out.normal = mat3(transpose(inverse(model))) * normal; // we only keep the scale and rotations from model matrix
    vs_out.tex_coords = tex_coords;

    /* ------------------------------------------------------- */
    /* ------------------------------------------------------- */

    vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
    vec3 B = normalize(vec3(model * vec4(bitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
    T = normalize(T - dot(T, N) * N);
    vs_out.TBN = mat3(T, B, N);

    /* ------------------------------------------------------- */
    /* ------------------------------------------------------- */

    gl_ClipDistance[0] = dot(vs_out.pos, clip_plane); // FIXME: use model matrix to modify clip_plane
    gl_Position = projection * view * vs_out.pos;

}