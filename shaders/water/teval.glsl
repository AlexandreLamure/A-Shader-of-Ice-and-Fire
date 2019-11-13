#version 450

layout (quads, equal_spacing, ccw) in;

in TCS_OUT
{
    vec4 pos;
    vec3 normal;
    vec2 tex_coords;
    vec3 tangent;
    vec3 bitangent;
} tes_in[];

out TES_OUT
{
    vec4 pos;
    vec3 normal;
    vec2 tex_coords;
    mat3 TBN;
    vec4 clip_space;
} tes_out;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 clip_plane;


void main()
{
    vec4 p1 = mix(gl_in[1].gl_Position, gl_in[0].gl_Position, gl_TessCoord.x);
    vec4 p2 = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);
    gl_Position = mix(p1, p2, gl_TessCoord.y);

    vec4 pos1 = mix(tes_in[1].pos, tes_in[0].pos, gl_TessCoord.x);
    vec4 pos2 = mix(tes_in[2].pos, tes_in[3].pos, gl_TessCoord.x);
    vec4 position = mix(pos1, pos2, gl_TessCoord.y);

    vec3 n1 = mix(tes_in[1].normal, tes_in[0].normal, gl_TessCoord.x);
    vec3 n2 = mix(tes_in[2].normal, tes_in[3].normal, gl_TessCoord.x);
    vec3 normal = mix(n1, n2, gl_TessCoord.y);

    vec2 t1 = mix(tes_in[1].tex_coords, tes_in[0].tex_coords, gl_TessCoord.x);
    vec2 t2 = mix(tes_in[2].tex_coords, tes_in[3].tex_coords, gl_TessCoord.x);
    vec2 tex_coords = mix(t1, t2, gl_TessCoord.y);

    vec3 ta1 = mix(tes_in[1].tangent, tes_in[0].tangent, gl_TessCoord.x);
    vec3 ta2 = mix(tes_in[2].tangent, tes_in[3].tangent, gl_TessCoord.x);
    vec3 tangent = mix(ta1, ta2, gl_TessCoord.y);

    vec3 b1 = mix(tes_in[1].bitangent, tes_in[0].bitangent, gl_TessCoord.x);
    vec3 b2 = mix(tes_in[2].bitangent, tes_in[3].bitangent, gl_TessCoord.x);
    vec3 bitangent = mix(b1, b2, gl_TessCoord.y);

    tes_out.pos = model * position;
    tes_out.normal = mat3(transpose(inverse(model))) * normal; // we only keep the scale and rotations from model matrix
    tes_out.tex_coords = tex_coords;

    /* ------------------------------------------------------- */
    /* ------------------------------------------------------- */

    vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
    vec3 B = normalize(vec3(model * vec4(bitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
    T = normalize(T - dot(T, N) * N);
    tes_out.TBN = mat3(T, B, N);

    gl_ClipDistance[0] = dot(tes_out.pos, clip_plane); // FIXME: use model matrix to modify clip_plane
    gl_Position = projection * view * tes_out.pos;

    tes_out.clip_space = projection * view * tes_out.pos;
}