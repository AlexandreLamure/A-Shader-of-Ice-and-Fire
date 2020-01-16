#version 450

struct PointLight
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 pos;

    float constant;
    float linear;
    float quadratic;
};


uniform bool ice_age;
uniform float ice_time;
uniform float wave_speed;
uniform float total_time;

float get_ice_state(vec4 position)
{
    const float transition_speed = 0.1;
    const float water_width = 45;
    const float decay = 0.5; // to reach the end faster
    float ice_state = (position.x / water_width + 1 + decay) * transition_speed;
    if (ice_age)
        return ice_state * (ice_time);
    else
        return -ice_state * (ice_time - 1);
}

float get_ice_wave(float ice_state)
{
    return 1 - step(0.05, ice_state);
}


const vec3 ice_light_color = vec3(1.f, 1.f, 10.f);
const vec3 ice_ambient_light_color = vec3(0.1f, 0.1f, 0.12f);

vec4 ice_light_colorize(vec4 color, vec4 position)
{
    const float transition_speed = 4;
    return mix(color, vec4(ice_light_color, 1), clamp(get_ice_state(position) * transition_speed, 0, 1));
}

PointLight ice_point_light_colorize(PointLight light, vec4 position)
{
    const float transition_speed = 4;
    PointLight colorized_light;
    float coef = clamp(get_ice_state(position) * transition_speed, 0, 1);

    colorized_light.ambient = mix(light.ambient, ice_ambient_light_color, coef);
    colorized_light.diffuse = mix(light.diffuse, ice_light_color, coef);
    colorized_light.specular= mix(light.specular, ice_light_color, coef);

    colorized_light.pos = light.pos;

    colorized_light.constant = light.constant;
    colorized_light.linear = light.linear;
    colorized_light.quadratic = light.quadratic;

    return colorized_light;
}