#version 410 core

uniform vec4 u_Bounds;
uniform float u_Depth;

layout(location = 0) in vec2 a_Position;

out vec2 v_TexCoords;

void main()
{
    vec2 pos = mix(u_Bounds.xy, u_Bounds.zw, a_Position);

    v_TexCoords = vec2(a_Position.x, 1.0 - a_Position.y);
    gl_Position = vec4(pos, u_Depth, 1.0);
}
