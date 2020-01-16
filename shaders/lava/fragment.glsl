#version 450

in GS_OUT
{
    vec4 pos;
    vec3 normal;
    vec2 tex_coords;
    vec4 clip_space;
} fs_in;

out vec4 output_color;

uniform float total_time;
uniform vec3 camera_pos;

// from lava_texture.glsl
vec4 lava_texture_mix(vec3 position);

// from ice.glsl
float get_ice_state(vec4 position);
float get_ice_wave(float ice_state);


void main()
{
    vec3 normal = fs_in.normal; // no normal map

    output_color = lava_texture_mix(fs_in.pos.xyz);

    // Ice Age color
    const float transition_speed = 4;
    float ice_color_decay = 2*clamp(get_ice_state(fs_in.pos) * transition_speed , 0, 1);

    // Cancel HDR luminosity
    // The color of lava texture does not compute lights, so the value is already between 0 and 1.
    output_color.rgb = -log(-output_color.rgb + 1.001 - ice_color_decay);

    // For biohazard mode
    //output_color.rgb = -log(-output_color.rgb + 1);
}