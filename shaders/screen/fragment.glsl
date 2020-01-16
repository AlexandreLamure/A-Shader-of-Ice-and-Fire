#version 450

in VS_OUT
{
    vec2 tex_coords;
} fs_in;

layout (location = 0) out vec4 output_color;
layout (location = 1) out vec4 BrightColor;

uniform sampler2D texture_other0; // Bloom FBO
uniform sampler2D texture_other1; // Blur FBO


uniform float total_time;

void main()
{
    const float gamma = 2.2;
    const float exposure = 1.5;

    vec3 hdr_color = texture(texture_other0, fs_in.tex_coords).rgb;
    vec3 blur_color = texture(texture_other1, fs_in.tex_coords).rgb;

    hdr_color += blur_color;

    //vec3 result = hdr_color / (hdr_color + vec3(1.0));
    vec3 result = vec3(1.0) - exp(-hdr_color * exposure);

    // funny mode
    //result = -log(- (-log(-result + 1)) + 1);

    //result = pow(result, vec3(1.0 / gamma));


    // for debug
    //if (fs_in.tex_coords.x > 0.5)
        output_color = vec4(result, 1.0);
    /*
    else if (fs_in.tex_coords.x == 0.5)
        output_color = vec4(1, 0, 0, 1);
    else
        output_color = texture(texture_other0, fs_in.tex_coords);
    */
}