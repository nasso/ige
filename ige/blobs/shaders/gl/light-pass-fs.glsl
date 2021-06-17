#version 410 core

#define LIGHT_TYPE_AMBIENT 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_DIRECTIONAL 2

uniform sampler2D u_GbufferAlbedo;
uniform sampler2D u_GbufferNormal;
uniform sampler2D u_GbufferDepth;
uniform mat4 u_InvProjection;
uniform int u_LightType;
uniform vec4 u_LightPosition;
uniform vec3 u_LightDiffuse;

in vec2 v_TexCoords;

out vec4 o_Color;

vec3 unproject(vec3 screen)
{
    vec4 view = u_InvProjection * vec4(screen.xyz, 1.0);

    return view.xyz / view.w;
}

vec3 calculate_light_factor(vec3 view_pos, vec3 normal)
{
    // cos_theta represents the incidence at which the light hits the surface:
    // 0.0 -> the light doesn't hit the surface (darkest)
    // 1.0 -> the light is perfectly aligned with the surface normal (brightest)
    // 0.0 .. 1.0 -> the light hits the surface with some angle
    // float cos_theta = max(dot(normal, sun_dir), 0.0);

    switch (u_LightType) {
    case LIGHT_TYPE_AMBIENT:
        return u_LightDiffuse.rgb;
    case LIGHT_TYPE_POINT: {
        vec3 light_vector = u_LightPosition.xyz - view_pos;
        float light_distance = length(light_vector);

        float cos_theta = max(dot(normal, normalize(light_vector)), 0.0);
        float falloff
            = 1.0 - pow(clamp(light_distance / u_LightPosition.w, 0.0, 1.0), 2);

        return u_LightDiffuse * cos_theta * falloff;
    }
    case LIGHT_TYPE_DIRECTIONAL: {
        float cos_theta = max(dot(normal, -u_LightPosition.xyz), 0.0);

        return u_LightDiffuse * cos_theta;
    }
    }
}

void main()
{
    float depth = texture(u_GbufferDepth, v_TexCoords).r;
    vec3 view_pos = unproject(vec3(v_TexCoords, depth) * 2.0 - 1.0);

    vec4 albedo = texture(u_GbufferAlbedo, v_TexCoords);
    vec3 normal = vec3(texture(u_GbufferNormal, v_TexCoords).xy, 0.0);

    // reconstruct normal z
    normal.z = sqrt(1.0 - dot(normal.xy, normal.xy));

    vec3 diffuse = albedo.rgb * calculate_light_factor(view_pos, normal);

    o_Color = vec4(diffuse, 1.0);
}
