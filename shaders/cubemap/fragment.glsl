#version 450

in vec3 tex_coords;

out vec4 output_color;

uniform samplerCube texture_cubemap;


void main()
{
    output_color = texture(texture_cubemap, tex_coords);
    //output_color = vec4(tex_coords, 1);
    output_color = mix(vec4(1), vec4(tex_coords, 1), 0.1);
}