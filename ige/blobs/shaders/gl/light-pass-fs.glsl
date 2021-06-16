#version 410 core

#define LIGHT_TYPE_AMBIENT 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_DIRECTIONAL 2

uniform sampler2D u_GbufferAlbedo;
uniform sampler2D u_GbufferNormal;
uniform int u_LightType;
uniform vec3 u_LightPosition;
uniform vec3 u_LightDiffuse;

in vec2 v_TexCoords;

out vec4 o_Color;

vec3 calculate_light_factor(vec3 normal)
{
    // cos_theta represents the incidence at which the light hits the surface:
    // 0.0 -> the light doesn't hit the surface (darkest)
    // 1.0 -> the light is perfectly aligned with the surface normal (brightest)
    // 0.0 .. 1.0 -> the light hits the surface with some angle
    // float cos_theta = max(dot(normal, sun_dir), 0.0);

    switch (u_LightType) {
    case LIGHT_TYPE_AMBIENT:
        return u_LightDiffuse.rgb;
    case LIGHT_TYPE_POINT:
        return vec3(0.0);
    case LIGHT_TYPE_DIRECTIONAL: {
        float cos_theta = max(dot(normal, -u_LightPosition), 0.0);

        return u_LightDiffuse * cos_theta;
    }
    }
}

void main()
{
    vec4 albedo = texture(u_GbufferAlbedo, v_TexCoords);
    vec3 normal = vec3(texture(u_GbufferNormal, v_TexCoords).xy, 0.0);

    // reconstruct normal z
    normal.z = sqrt(1.0 - dot(normal.xy, normal.xy));

    vec3 diffuse = albedo.rgb * calculate_light_factor(normal);

    o_Color = vec4(diffuse, 1.0);
}
