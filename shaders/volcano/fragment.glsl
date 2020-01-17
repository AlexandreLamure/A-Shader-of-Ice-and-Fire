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



in GS_OUT
{
    vec4 pos;
    vec3 normal;
    vec2 tex_coords;
    mat3 TBN;
} fs_in;

out vec4 output_color;


#define NB_DIR_LIGHTS 2
#define NB_POINT_LIGHTS 2

uniform DirLight dir_lights[NB_DIR_LIGHTS];
uniform PointLight point_lights[NB_POINT_LIGHTS];


uniform sampler2D texture_ambient1;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

uniform float total_time;
uniform vec3 camera_pos;
uniform float water_h;

float get_ice_state(vec4 position);
float get_ice_wave(float ice_state);
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

vec3 compute_point_light(PointLight light, Material material, vec3 normal, vec3 camera_dir)
{
    vec3 light_dir = normalize(light.pos - fs_in.pos.xyz);
    // diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    // specular shading
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(camera_dir, reflect_dir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.pos - fs_in.pos.xyz);
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

vec3 compute_lights(Material material, vec3 normal)
{
    vec3 camera_dir = normalize(camera_pos - fs_in.pos.xyz);
    vec3 light_color = vec3(0);

    // Directional lighting
    for(int i = 0; i < NB_DIR_LIGHTS; i++)
        light_color += compute_dir_light(dir_lights[i], material, normal, camera_dir);
    // Point lights
    for(int i = 0; i < NB_POINT_LIGHTS; i++)
    {
        PointLight colorized_light = ice_point_light_colorize(point_lights[i], fs_in.pos);
        light_color += compute_point_light(colorized_light, material, normal, camera_dir);
    }

    return light_color;
}


void main()
{
    vec3 normal = fs_in.normal; // if no normal map
    /*vec3 normal = texture(texture_normal1, fs_in.tex_coords).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(fs_in.TBN * normal);
*/
    /* ------------------------------------------------------- */
    /* ------------------------------------------------------- */

    Material material;
    vec4 diffuse = texture(texture_diffuse1, fs_in.tex_coords);
    material.ambient = vec3(texture(texture_ambient1, fs_in.tex_coords));
    material.diffuse = vec3(diffuse);
    material.specular = vec3(texture(texture_specular1, fs_in.tex_coords));
    material.shininess = 20; //FIXME: get value from assimp

    output_color = vec4(compute_lights(material, normal), diffuse.a);

    // Decrease light under water
    if (fs_in.pos.y < water_h)
        output_color.rgb *= vec3(0.4, 0.42, 0.5);
}