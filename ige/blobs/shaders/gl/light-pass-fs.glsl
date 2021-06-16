#version 410 core

uniform sampler2D u_GbufferAlbedo;
uniform sampler2D u_GbufferNormal;

in vec2 v_TexCoords;

out vec4 o_Color;

vec3 calculate_light_factor(vec3 normal)
{
    // hardcoded sunlight!
    vec3 ambient_light = vec3(0.25);
    vec3 sun_color = vec3(1.0);
    vec3 sun_dir = normalize(vec3(0.85, 0.8, 0.75));

    // cos_theta represents the incidence at which the light hits the surface:
    // 0.0 -> the light doesn't hit the surface (darkest)
    // 1.0 -> the light is perfectly aligned with the surface normal (brightest)
    // 0.0 .. 1.0 -> the light hits the surface with some angle
    float cos_theta = max(dot(normal, sun_dir), 0.0);

    return ambient_light + (sun_color * cos_theta);
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
