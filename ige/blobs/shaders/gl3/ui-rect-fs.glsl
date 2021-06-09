#version 410 core

uniform vec4 u_FillColor;

out vec4 o_Color;

void main()
{
    vec4 base_color = u_FillColor;

    o_Color = base_color;
}
