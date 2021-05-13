#version 330 core

in vec2 v_TexCoords;

out vec4 o_Color;

void main()
{
    o_Color = vec4(v_TexCoords, 0.5, 1.0);
}
