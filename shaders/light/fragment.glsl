#version 450 core

layout (location = 0) out vec4 output_color;
//layout (location = 1) out vec4 BrightColor;

in VS_OUT {
    vec4 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform vec3 lightColor;

float get_ice_state(vec4 position);
float get_ice_wave(float ice_state);


void main()
{
    output_color = vec4(lightColor, 1.0);

    // Ice Age color
    const float transition_speed = 4;
    output_color = mix(output_color, vec4(1.f, 1.f, 10.f, 1), clamp(get_ice_state(fs_in.FragPos) * transition_speed, 0, 1));


    /*
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
    BrightColor = vec4(FragColor.rgb, 1.0);
    else
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0); */

}