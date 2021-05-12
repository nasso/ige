#version 330 core

in vec3 v_Normal;

out vec4 o_Color;

void main()
{
    o_Color = vec4(v_Normal, 1.0);
}
