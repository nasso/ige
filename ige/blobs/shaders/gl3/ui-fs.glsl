#version 410 core

uniform vec4 u_BaseColorFactor;

in vec2 v_TexCoords;

out vec4 o_Color;

void main()
{
    vec4 base_color = u_BaseColorFactor;

    o_Color = base_color;
}
