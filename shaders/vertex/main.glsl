#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex_coords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out vec4 interpolated_pos;
out vec3 interpolated_normal;
out vec4 interpolated_color;
out vec2 interpolated_tex_coords;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int mesh_id;
uniform float total_time;
uniform int rand;


void main()
{
    interpolated_pos = model * vec4(position, 1);
    interpolated_normal = mat3(transpose(inverse(model))) * normal; // we only keep the scale and rotations from model matrix
    interpolated_tex_coords = tex_coords;

    /* ------------------------------------------------------- */
    /* ------------------------------------------------------- */

    vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
    vec3 B = normalize(vec3(model * vec4(bitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
    T = normalize(T - dot(T, N) * N);
    TBN = mat3(T, B, N);

    gl_Position = projection * view * interpolated_pos;

}