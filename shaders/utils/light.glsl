#version 450

struct Material // Use vec3 instead of sampler2D to avoid expensive copy of data
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float shininess;
};

struct DirLight
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 dir;
};

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


#define NB_DIR_LIGHTS 2
#define NB_LIGHT_MODELS 4
#define NB_POINT_LIGHTS 10

uniform DirLight dir_lights[NB_DIR_LIGHTS];
uniform PointLight point_lights[NB_LIGHT_MODELS + NB_POINT_LIGHTS];

uniform float total_time;
uniform vec3 camera_pos;

// from ice.glsl
float get_ice_state(vec4 position);
PointLight ice_point_light_colorize(PointLight light, vec4 position);

vec3 compute_dir_light(DirLight light, Material material, vec3 normal, vec3 camera_dir)
{
    vec3 light_dir = normalize(-light.dir);
    // diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    // specular shading
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(camera_dir, reflect_dir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * material.ambient;
    vec3 diffuse  = light.diffuse  * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    return (ambient + diffuse + specular);
}

vec3 compute_point_light(PointLight light, Material material, vec3 normal, vec3 camera_dir, vec3 pos)
{
    vec3 light_dir = normalize(light.pos - pos);
    // diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    // specular shading
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(camera_dir, reflect_dir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.pos - pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient  = light.ambient  * material.ambient;
    vec3 diffuse  = light.diffuse  * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 compute_lights(Material material, vec3 normal, vec4 pos, vec2 tex_coords)
{
    vec3 camera_dir = normalize(camera_pos - pos.xyz);
    vec3 light_color = vec3(0);

    // Directional lighting
    for(int i = 0; i < NB_DIR_LIGHTS; i++)
    light_color += compute_dir_light(dir_lights[i], material, normal, camera_dir);
    // Point lights
    for(int i = 0; i < NB_LIGHT_MODELS; i++)
    {
        PointLight colorized_light = ice_point_light_colorize(point_lights[i], pos);
        light_color += compute_point_light(colorized_light, material, normal, camera_dir, pos.xyz);
    }
    float ice_state = clamp(get_ice_state(pos), 0, 1);
    const float decrease_speed = 5;
    ice_state = clamp(ice_state*decrease_speed,0,1);
    if (ice_state != 1)
    {
        for (int i = NB_LIGHT_MODELS; i < NB_LIGHT_MODELS + NB_POINT_LIGHTS; i++)
        {
            PointLight colorized_light = ice_point_light_colorize(point_lights[i], pos);
            colorized_light.diffuse.r += colorized_light.diffuse.r * 0.3 * cos(total_time*1.5+tex_coords.x*tex_coords.y*3.14);
            light_color += compute_point_light(colorized_light, material, normal, camera_dir, pos.xyz) * (1 - ice_state);
        }
    }

    return light_color;
}