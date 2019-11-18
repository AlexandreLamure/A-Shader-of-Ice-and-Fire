#version 450

vec3 mod289(vec3 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float random(float x)
{
    return fract(sin(x)*1e4);
}

float random(vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9898,78.233)))* 43758.5453123);
}

float pattern(vec2 st, vec2 v, float t)
{
    vec2 p = floor(st+v);
    return step(t, random(100.+p*.000001)+random(p.x)*0.5 );
}