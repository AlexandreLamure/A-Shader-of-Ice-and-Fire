#version 450

uniform bool ice_age;
uniform float ice_time;
uniform float wave_speed;
uniform float total_time;

float get_ice_state(vec4 position)
{
    const float transition_speed = 0.1;
    const float water_width = 45;
    const float decay = 1.5; // to reach the end faster
    float ice_state = (position.x / water_width + decay) * transition_speed;
    if (ice_age)
        return ice_state * (ice_time);
    else
        return -ice_state * (ice_time - 1);
}

float get_ice_wave(float ice_state)
{
    return 1 - step(0.05, ice_state);
}
