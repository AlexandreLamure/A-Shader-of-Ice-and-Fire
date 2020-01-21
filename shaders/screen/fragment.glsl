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
uniform float ice_time;
uniform bool ice_age;

void main()
{
    const float gamma = 2.2;
    const float exposure = 1.5;

    vec3 hdr_color = texture(texture_other0, fs_in.tex_coords).rgb;
    vec3 blur_color = texture(texture_other1, fs_in.tex_coords).rgb;

    hdr_color += blur_color;

    vec3 result = vec3(1.0) - exp(-hdr_color * exposure);

    // funny mode
    //result = -log(- (-log(-result + 1)) + 1);

    //result = pow(result, vec3(1.0 / gamma));


    // Draw circle
    if (ice_age && ice_time < 4)
    {
        vec2 resolution = textureSize(texture_other0, 0);
        float width = resolution.x;
        float height = resolution.y;
        float u = fs_in.tex_coords.x * 2 - 1;
        float v = fs_in.tex_coords.y * 2 - 1;
        float display_coef = width / height;
        float r2 = pow(u * display_coef, 2) + pow(v, 2);
        float decay_start = 1;
        float speed = 3.5;
        float initial_size = 1;
        float range = 3.5;
        float circle_min = (ice_time*2) * range - decay_start;
        float circle_max = circle_min + initial_size + (1 - ice_time)*speed*0.5 - decay_start;
        if (r2 > circle_min && r2 < circle_max)
            result = vec3(1) - result;
    }

    output_color = vec4(result, 1.0);
}