#version 450

layout (triangles, equal_spacing, ccw) in;

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
    gl_Position = gl_in[0].gl_Position * gl_TessCoord.x
                + gl_in[1].gl_Position * gl_TessCoord.y
                + gl_in[2].gl_Position * gl_TessCoord.z;
    gl_Position = projection * view * model * gl_Position;

    vec4 position = tes_in[0].pos * gl_TessCoord.x
                    + tes_in[1].pos * gl_TessCoord.y
                    + tes_in[2].pos * gl_TessCoord.z;

    vec3 normal = tes_in[0].normal * gl_TessCoord.x
                + tes_in[1].normal * gl_TessCoord.y
                + tes_in[2].normal * gl_TessCoord.z;

    vec2 tex_coords = tes_in[0].tex_coords * gl_TessCoord.x
                    + tes_in[1].tex_coords * gl_TessCoord.y
                    + tes_in[2].tex_coords * gl_TessCoord.z;

    vec3 tangent = tes_in[0].tangent * gl_TessCoord.x
                + tes_in[1].tangent * gl_TessCoord.y
                + tes_in[2].tangent * gl_TessCoord.z;

    vec3 bitangent = tes_in[0].bitangent * gl_TessCoord.x
                    + tes_in[1].bitangent * gl_TessCoord.y
                    + tes_in[2].bitangent * gl_TessCoord.z;

    tes_out.pos = model * position;
    tes_out.normal = mat3(transpose(inverse(model))) * normal; // we only keep the scale and rotations from model matrix
    tes_out.tex_coords = tex_coords;
    tes_out.clip_space = projection * view * tes_out.pos;
    gl_ClipDistance[0] = dot(tes_out.pos, clip_plane); // FIXME: use model matrix to modify clip_plane

    /* ------------------------------------------------------- */
    /* ------------------------------------------------------- */

    vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
    vec3 B = normalize(vec3(model * vec4(bitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
    T = normalize(T - dot(T, N) * N);
    tes_out.TBN = mat3(T, B, N);
}