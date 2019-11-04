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
    vec4 clip_space;
} fs_in;

out vec4 output_color;


#define NB_DIR_LIGHTS 2
#define NB_POINT_LIGHTS 2

uniform DirLight dir_lights[NB_DIR_LIGHTS];
uniform PointLight point_lights[NB_POINT_LIGHTS];


uniform sampler2D texture_ambient1;
uniform sampler2D texture_diffuse1; // DuDv Map
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

uniform sampler2D texture_other0; // Reflection texture
uniform sampler2D texture_other1; // Refraction texture
uniform sampler2D texture_other2; // Refraction rbo


uniform float total_time;
uniform vec2 resolution;
uniform vec3 camera_pos;
uniform int mesh_id;
uniform int rand;
uniform float wave_speed;


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
        light_color += compute_point_light(point_lights[i], material, normal, camera_dir);
    // Spot light
    //for(int i = 0; i < NB_SPOT_LIGHTS; i++)
        //light_color += compute_spot_light(spot_lights[i], material, normal, camera_dir);

    return light_color;
}

vec4 compute_water_texture(vec2 distortion)
{
    // Compute Texture coordinates
    vec2 ndc = (fs_in.clip_space.xy / fs_in.clip_space.w) / 2.f + 0.5f;
    vec2 reflect_tex_coords = vec2(ndc.x, -ndc.y);
    vec2 refract_tex_coords = ndc;

    // Add distortion and clamp
    reflect_tex_coords.x = clamp(reflect_tex_coords.x + distortion.x, 0.001f, 0.999f);
    reflect_tex_coords.y = clamp(reflect_tex_coords.y + distortion.y, -0.999f, -0.001f);
    refract_tex_coords = clamp(refract_tex_coords + distortion, 0.001f, 0.999f);

    // Get texture color
    vec4 reflect = texture(texture_other0, reflect_tex_coords);
    vec4 refract = texture(texture_other1, refract_tex_coords);

    // Fresnel
    vec3 camera_dir = normalize(camera_pos - fs_in.pos.xyz);
    float fresnel_coef = dot(camera_dir, vec3(0,1,0));
    fresnel_coef = pow(fresnel_coef, 1.2);

    // Combine reflect & refract
    return mix(reflect, refract, fresnel_coef);
}


void main()
{
    output_color = vec4(1);

    /* ------------------------------------------------------- */
    /* ------------------------------------------------------- */

    // Add distortion to texture coordinates
    const float wave_strength = 0.02;
    vec2 distorted_tex_coords = vec2(fs_in.tex_coords.x + wave_speed, fs_in.tex_coords.y);
    distorted_tex_coords = texture(texture_diffuse1, distorted_tex_coords).rg * 0.1;
    distorted_tex_coords = fs_in.tex_coords + distorted_tex_coords;
    distorted_tex_coords.y += wave_speed;
    vec2 distortion = texture(texture_diffuse1, distorted_tex_coords).rg * 2.0 - 1.0;
    distortion *= wave_strength;

    /* ------------------------------------------------------- */
    /* ------------------------------------------------------- */

    //vec3 normal = fs_in.normal; // if no normal map
    vec3 normal = texture(texture_normal1, distorted_tex_coords).rgb;
    normal *= 0.5;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(fs_in.TBN * normal);

    /* ------------------------------------------------------- */
    /* ------------------------------------------------------- */

    Material material;

    vec4 diffuse = compute_water_texture(distortion);

    material.ambient = vec3(diffuse);
    material.diffuse = vec3(diffuse);
    material.specular = vec3(diffuse);
    material.shininess = 20; //FIXME: get value from assimp

    output_color *= vec4(compute_lights(material, normal), diffuse.a);

    /* ------------------------------------------------------- */
    /* ------------------------------------------------------- */

    // Add blue tint
    output_color = mix(output_color, vec4(0.0, 0.3, 0.5, 0.8), 0.15);
}