#version 450

in VS_OUT
{
    vec2 tex_coords;
} fs_in;

layout (location = 0) out vec4 output_color;
layout (location = 1) out vec4 bright_color;

uniform sampler2D texture_other0; // Scene FBO

void main()
{
    vec3 hdr_color = texture(texture_other0, fs_in.tex_coords).rgb;

    output_color = vec4(hdr_color, 1.0);

    float brightness = dot(hdr_color, vec3(0.2126, 0.7152, 0.0722));

    if (brightness > 1.0)
        bright_color = vec4(hdr_color, 1);
    else
        bright_color = vec4(0, 0, 0, 1);
}