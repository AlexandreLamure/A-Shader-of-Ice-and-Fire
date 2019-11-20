#version 450

layout (triangles, equal_spacing, ccw) in;

in TCS_OUT
{
    vec4 pos;
    vec3 normal;
    vec2 tex_coords;
} tes_in[];

out TES_OUT
{
    vec4 pos;
    vec3 normal;
    vec2 tex_coords;
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


    tes_out.pos = model * position;
    tes_out.normal = mat3(transpose(inverse(model))) * normal; // we only keep the scale and rotations from model matrix
    tes_out.tex_coords = tex_coords;
    tes_out.clip_space = projection * view * tes_out.pos;
    gl_ClipDistance[0] = dot(tes_out.pos, clip_plane); // FIXME: use model matrix to modify clip_plane
}