#version 410 core

uniform vec4 u_BaseColorFactor;
uniform bool u_HasBaseColorTexture;
uniform sampler2D u_BaseColorTexture;

in vec3 v_Normal;
in vec2 v_TexCoords;

out vec4 o_Color;

vec3 calculate_light_factor()
{
    // hardcoded sunlight!
    vec3 ambient_light = vec3(0.25);
    vec3 sun_color = vec3(1.0);
    vec3 sun_dir = normalize(vec3(0.85, 0.8, 0.75));

    // cos_theta represents the incidence at which the light hits the surface:
    // 0.0 -> the light doesn't hit the surface (darkest)
    // 1.0 -> the light is perfectly aligned with the surface normal (brightest)
    // 0.0 .. 1.0 -> the light hits the surface with some angle
    float cos_theta = max(dot(normalize(v_Normal), sun_dir), 0.0);

    return ambient_light + (sun_color * cos_theta);
}

void main()
{
    vec4 base_color = u_BaseColorFactor;

    if (u_HasBaseColorTexture) {
        base_color *= texture(u_BaseColorTexture, v_TexCoords);
    }

    vec3 diffuse = base_color.rgb * calculate_light_factor();

    o_Color = vec4(diffuse, 1.0);
}
