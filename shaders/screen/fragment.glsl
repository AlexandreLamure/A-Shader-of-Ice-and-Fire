#version 450

in VS_OUT
{
    vec2 tex_coords;
} fs_in;

out vec4 output_color;


uniform sampler2D texture_other0;

uniform float total_time;
uniform vec2 resolution;

uniform vec3 camera_pos;
uniform int mesh_id;
uniform int rand;


void main()
{

    const float gamma = 2.2;
    const float exposure = 0.5;

    output_color = vec4(1);

    /* ------------------------------------------------------- */
    /* ------------------------------------------------------- */

    //output_color *= texture(texture_other0, fs_in.tex_coords);

    vec3 hdrColor = texture(texture_other0, fs_in.tex_coords).rgb;

    vec3 result = hdrColor / (hdrColor + vec3(1.0));
    //vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

    //result = pow(result, vec3(1.0 / gamma));

    output_color *= vec4(result, 1.0);
}