#version 410 core

uniform vec4 u_Bounds;
uniform float u_Depth;

layout(location = 0) in vec2 a_Position;

void main()
{
    vec2 pos = mix(u_Bounds.xy, u_Bounds.zw, a_Position);

    gl_Position = vec4(pos, u_Depth, 1.0);
}
