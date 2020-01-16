#version 450

in VS_OUT {
    vec4 pos;
    vec3 normal;
    vec2 tex_coords;
} fs_in;

layout (location = 0) out vec4 output_color;
//layout (location = 1) out vec4 BrightColor;

uniform vec3 light_color;

vec4 ice_light_colorize(vec4 color, vec4 position);

void main()
{
    output_color = vec4(light_color, 1.0);

    // Ice Age color
    output_color = ice_light_colorize(output_color, fs_in.pos);

    /*
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
    BrightColor = vec4(FragColor.rgb, 1.0);
    else
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0); */

}