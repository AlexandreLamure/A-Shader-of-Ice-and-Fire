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
#define NB_LIGHT_MODELS 2
#define NB_POINT_LIGHTS 10

uniform DirLight dir_lights[NB_DIR_LIGHTS];
uniform PointLight point_lights[NB_LIGHT_MODELS + NB_POINT_LIGHTS];


uniform sampler2D texture_ambient1;
uniform sampler2D texture_diffuse1; // DuDv Map
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

uniform sampler2D texture_other0; // Reflection texture
uniform sampler2D texture_other1; // Refraction texture
uniform sampler2D texture_other2; // Refraction depth texture

uniform float total_time;
uniform vec3 camera_pos;
uniform float wave_speed;


// from ice.glsl
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
    for(int i = 0; i < NB_LIGHT_MODELS; i++)
    {
        PointLight colorized_light = ice_point_light_colorize(point_lights[i], fs_in.pos);
        light_color += compute_point_light(colorized_light, material, normal, camera_dir);
    }
    float ice_state = clamp(get_ice_state(fs_in.pos), 0, 1);
    const float decrease_speed = 5;
    ice_state = clamp(ice_state*decrease_speed,0,1);
    if (ice_state != 1)
    {
        for (int i = NB_LIGHT_MODELS; i < NB_LIGHT_MODELS + NB_POINT_LIGHTS; i++)
        {
            PointLight colorized_light = ice_point_light_colorize(point_lights[i], fs_in.pos);
            colorized_light.diffuse.r += colorized_light.diffuse.r * 0.3 * cos(total_time*1.5+fs_in.tex_coords.x*fs_in.tex_coords.y*3.14);
            light_color += compute_point_light(colorized_light, material, normal, camera_dir) * (1 - ice_state);
        }
    }

    return light_color;
}

float get_water_depth(vec2 refract_tex_coords)
{
    float zNear = 0.1;
    float zFar  = 1000.0;
    float depth = texture(texture_other2, refract_tex_coords).r;
    //return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
    float floor_distance = 2.0 * zNear * zFar / (zFar + zNear - (2.0 * depth - 1.0) * (zFar - zNear));
//    water_distance = (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
    float water_distance = 2.0 * zNear * zFar / (zFar + zNear - (2.0 * gl_FragCoord.z - 1.0) * (zFar - zNear));
//    floor_distance = (2.0 * zNear) / (zFar + zNear - gl_FragCoord.z * (zFar - zNear));
    float water_depth = floor_distance - water_distance;
    return water_depth;
}

vec4 compute_water_texture(vec3 normal, vec2 distortion, float water_depth, float ice_state)
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

    // Darker when deeper
    refract = mix(refract, vec4(0,0.04,0.2, 0.9), clamp(water_depth / 60, 0, 1));


    // Fresnel
    vec3 camera_dir = normalize(camera_pos - fs_in.pos.xyz);
    float fresnel_coef = dot(camera_dir, normal);
    fresnel_coef = pow(fresnel_coef, 1.8);

    // Combine reflect & refract
    float coef = clamp(fresnel_coef - clamp(ice_state, 0, 0.2), 0, 1); // Increase reflection when Ice Age
    return mix(reflect, refract, coef);
}

void main()
{
    float ice_state = get_ice_state(fs_in.pos);
    float new_wave_speed = wave_speed * get_ice_wave(ice_state);

    /* ------------------------------------------------------- */
    /* ------------------------------------------------------- */

    // Depth
    vec2 refract_tex_coords = (fs_in.clip_space.xy / fs_in.clip_space.w) / 2.f + 0.5f;
    float water_depth = get_water_depth(refract_tex_coords);

    // Add distortion to texture coordinates
    const float wave_strength = 0.02;
    vec2 distorted_tex_coords = vec2(fs_in.tex_coords.x + new_wave_speed, fs_in.tex_coords.y);
    distorted_tex_coords = texture(texture_diffuse1, distorted_tex_coords).rg * 0.1;
    distorted_tex_coords = fs_in.tex_coords + distorted_tex_coords;
    distorted_tex_coords.y += new_wave_speed;
    vec2 distortion = texture(texture_diffuse1, distorted_tex_coords).rg * 2.0 - 1.0;
    distortion *= wave_strength;
    // Soften distortion on edges
    distortion *= clamp(water_depth / 10, 0, 1);

    /* ------------------------------------------------------- */
    /* ------------------------------------------------------- */

    //vec3 normal = fs_in.normal; // if no normal map
    vec3 normal = texture(texture_normal1, distorted_tex_coords).rgb;
    normal.b *= 8; // to smooth the water surface

    // Ice transition
    if (ice_state * normal.x * normal.y * normal.z > 0.15)
        normal.b *= clamp(ice_state, 0, 1);

    normal = vec3(normal * 2.0 - 1.0);
    normal = normalize(normal);
    normal = normalize(fs_in.TBN * normal);

    /* ------------------------------------------------------- */
    /* ------------------------------------------------------- */

    Material material;

    vec4 diffuse = compute_water_texture(normal, distortion, water_depth, ice_state);

    material.ambient = vec3(diffuse);
    material.diffuse = vec3(diffuse);
    material.specular = vec3(diffuse); // * 2
    material.shininess = 20; //FIXME: get value from assimp

    output_color = vec4(compute_lights(material, normal), diffuse.a);

    /* ------------------------------------------------------- */
    /* ------------------------------------------------------- */

    // Add blue tint
    output_color = mix(output_color, vec4(0., 0.15, 0.3, 0.8), 0.15);

    // Ice Age color
    output_color = mix(output_color, vec4(0.7, 0.9, 1., 0.95), clamp(ice_state, 0, 0.75));

    // FIXME
    //output_color.rgb = vec3(1.0) - exp(-output_color.rgb);
    //output_color.a = 1;

    // Soften edges
    output_color.a = clamp(water_depth / 5, 0, 1);
}