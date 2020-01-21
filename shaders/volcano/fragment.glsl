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
#define NB_LIGHT_MODELS 2
#define NB_POINT_LIGHTS 10

uniform DirLight dir_lights[NB_DIR_LIGHTS];
uniform PointLight point_lights[NB_LIGHT_MODELS + NB_POINT_LIGHTS];


uniform sampler2D texture_ambient1;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

uniform float total_time;
uniform vec3 camera_pos;
uniform vec3 water_limits;
uniform float ice_time;

float get_ice_state(vec4 position);
float get_ice_wave(float ice_state);
float snoise(vec3 v);
vec3 compute_lights(Material material, vec3 normal, vec4 pos, vec2 tex_coords);

vec3 compute_snow()
{
    // Snow pattern
    float snow = snoise(fs_in.pos.xyz);
    // Increase size over time
    snow = snow - 1 + clamp(ice_time*0.01,0,1.2);
    // Avoid too much bloom
    snow = clamp(snow, 0, 0.95);

    vec3 color = vec3(snow, snow, snow);
    float ice_coef = clamp(get_ice_state(fs_in.pos), 0, 0.5)*2;
    return color * ice_coef;
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

    output_color = vec4(compute_lights(material, normal, fs_in.pos, fs_in.tex_coords), diffuse.a);

    vec3 snow = compute_snow();
    // Decrease snow under water
    snow *= clamp(fs_in.pos.y - water_limits.y+0.2, 0, 1);
    output_color.rgb += snow;

    // Decrease light under water
    if (fs_in.pos.y < water_limits.y && !(fs_in.pos.z < water_limits.z && fs_in.pos.x > water_limits.x))
        output_color.rgb = mix(output_color.rgb, output_color.rgb * vec3(0.4, 0.42, 0.7), min(water_limits.y+0.1 - fs_in.pos.y, 1));
}