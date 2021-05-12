#version 330 core

uniform mat4 u_ProjViewModel;

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;

void main()
{
    gl_Position = u_ProjViewModel * vec4(a_Position, 1.0);
}
