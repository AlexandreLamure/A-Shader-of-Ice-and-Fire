#version 450

in vec3 tex_coords;

out vec4 output_color;

uniform samplerCube texture_cubemap;


void main()
{
    output_color = texture(texture_cubemap, tex_coords);

    // Cancel HDR luminosity
    // The color of cubemap texture does not compute lights, so the value is already between 0 and 1.
    output_color.rgb = -log(-output_color.rgb + 1.0001);

}