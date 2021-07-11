#version 410 core

uniform vec4 u_BaseColorFactor;
uniform bool u_HasBaseColorTexture;
uniform sampler2D u_BaseColorTexture;

in vec3 v_Normal;
in vec2 v_TexCoords;

layout(location = 0) out vec4 o_Albedo;
layout(location = 1) out vec2 o_Normal;

// spheremap transform (used by Cry Engine 3)
// see: https://aras-p.info/texts/CompactNormalStorage.html
vec2 encode_normal(vec3 n)
{
    float p = sqrt(n.z * 8.0 + 8.0);
    return n.xy / p + 0.5;
}

void main()
{
    vec4 base_color = u_BaseColorFactor;

    if (u_HasBaseColorTexture) {
        base_color *= texture(u_BaseColorTexture, v_TexCoords);
    }

    o_Normal = encode_normal(normalize(v_Normal));
    o_Albedo = vec4(base_color.rgb, 1.0);
}
