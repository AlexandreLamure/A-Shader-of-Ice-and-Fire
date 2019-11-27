#version 450

in VS_OUT
{
    vec2 tex_coords;
} fs_in;

layout (location = 0) out vec4 output_color;
layout (location = 1) out vec4 BrightColor;

uniform sampler2D texture_other0; // Bloom FBO
uniform sampler2D texture_other1; // Blur FBO


void main()
{
    const float gamma = 2.2;
    const float exposure = 1.5;

    //output_color = texture(texture_other0, fs_in.tex_coords);

    vec3 hdrColor = texture(texture_other0, fs_in.tex_coords).rgb;
    vec3 BlurColor = texture(texture_other1, fs_in.tex_coords).rgb;

    hdrColor += BlurColor;

    //vec3 result = hdrColor / (hdrColor + vec3(1.0));
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

    //result = pow(result, vec3(1.0 / gamma));

    output_color = vec4(result, 1.0);
}