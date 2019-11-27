#version 450

in VS_OUT
{
    vec2 tex_coords;
} fs_in;

layout (location = 0) out vec4 output_color;
layout (location = 1) out vec4 BrightColor;

uniform sampler2D texture_other0; // Screen FBO

void main()
{
    //output_color = texture(texture_other0, fs_in.tex_coords);

    vec3 hdrColor = texture(texture_other0, fs_in.tex_coords).rgb;

    output_color = vec4(hdrColor, 1.0);

    float brightness = dot(hdrColor, vec3(0.2126, 0.7152, 0.0722));

    if(brightness > 1.0)
        BrightColor = vec4(hdrColor, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}