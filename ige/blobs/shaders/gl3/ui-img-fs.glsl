#version 410 core

uniform vec4 u_Tint;
uniform sampler2D u_Texture;

in vec2 v_TexCoords;

out vec4 o_Color;

void main()
{
    vec4 base_color = u_Tint * texture(u_Texture, v_TexCoords);

    o_Color = base_color;
}
