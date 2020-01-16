#version 450

uniform float total_time;

// From misc.glsl
vec3 mod289(vec3 x);
vec2 mod289(vec2 x);
float random (float x);
float random (vec2 st);
float pattern(vec2 st, vec2 v, float t);

// From simplex.glsl
float snoise(vec3 v);

float lava_texture(vec3 position)
{
    const float flow_speed = 0.4;
    const float zoom = 1.;
    const vec3 center_pos = vec3(12.3, 10, -10);

    vec3 pos = position / 2.;

    float DF = 0.0; //2.0

    vec3 vel = vec3(total_time*.1)*zoom;
    vel.z = - vel.z;
    DF += snoise(pos+vel)*.25+.25;

    vel = vec3(length(pos - center_pos) - total_time * flow_speed)*zoom;
    vel.z = - vel.z;
    DF += snoise(pos+vel)*.25+.25;

    return DF;
}

vec4 lava_texture_real(vec3 position)
{
    float DF = lava_texture(position);

    // Realistic colors
    float r = smoothstep(.2, 1.75, fract(DF)/cos(DF));
    float g = smoothstep(.3, .75, fract(DF));
    float b = smoothstep(.1, .75, fract(DF));

    vec3 color = vec3(r, g, b);

    return vec4(1.0-color, 1.0);
}

vec4 lava_texture_mix(vec3 position)
{
    float DF = lava_texture(position);

    // Realistic colors
    float r1 = smoothstep(.2, 1.75, fract(DF)/cos(DF));
    float g1 = smoothstep(.3, .75, fract(DF));
    float b1 = smoothstep(.1, .75, fract(DF));
    // Cartoon colors
    float r2 = step(0.99, fract(DF)/cos(DF));
    float g2 = step(0.35, fract(DF));
    float b2 = step(0.22, fract(DF));
    // Blend & clamp
    float r = clamp(mix(r1, r2, 0.5), 0, 1);
    float g = clamp(mix(g1, g2, 0.3), 0, 1);
    float b = clamp(mix(b1, b2, 0.9), 0, 1);

    vec3 color = vec3(r, g, b);

    return vec4(1.0-color, 1.0);
}