#version 450

float get_ice_state(vec4 position, float total_time, float wave_speed)
{
    const float start_time = 5;
    const float transition_speed = 0.1;
    const float water_width = 45;
    return (position.x / water_width + 1.5) * ((total_time - start_time) * transition_speed);
}

float get_ice_wave(float ice_state)
{
    return 1 - step(0.05, ice_state);
}
