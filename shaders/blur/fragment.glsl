#version 450 core

in VS_OUT
{
    vec2 tex_coords;
} fs_in;

out vec4 output_color;

uniform sampler2D texture_other0; // BrightMap
uniform bool horizontal;


const float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162); // Gaussian coef

void main()
{
    vec2 tex_offset = 1.0 / textureSize(texture_other0, 0); // gets size of single texel
    vec3 result = texture(texture_other0, fs_in.tex_coords).rgb * weight[0]; // current fragment's contribution

    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(texture_other0, fs_in.tex_coords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(texture_other0, fs_in.tex_coords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(texture_other0, fs_in.tex_coords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(texture_other0, fs_in.tex_coords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    output_color = vec4(result, 1.0);
}