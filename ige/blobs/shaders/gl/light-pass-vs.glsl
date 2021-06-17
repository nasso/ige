#version 410 core

layout(location = 0) in vec2 a_Position;

out vec2 v_TexCoords;

void main()
{
    v_TexCoords = a_Position;
    gl_Position = vec4(a_Position * 2.0 - 1.0, 0.0, 1.0);
}
