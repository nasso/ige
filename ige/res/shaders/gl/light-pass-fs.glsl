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

vec3 decode_normal(vec2 enc)
{
    vec2 fenc = enc * 4.0 - 2.0;
    float f = dot(fenc, fenc);
    float g = sqrt(1.0 - f / 4.0);

    return vec3(fenc * g, 1.0 - f / 2.0);
}

// the incidence at which the light hits the surface:
// 0.0 -> the light doesn't hit the surface (darkest)
// 1.0 -> the light is perfectly aligned with the surface normal (brightest)
// 0.0 .. 1.0 -> the light hits the surface with some angle
// float cos_theta = max(dot(normal, sun_dir), 0.0);
//
// note: dir must be normalised!
float incidence_factor(vec3 normal, vec3 dir)
{
    return max(dot(normal, dir), 0.0);
}

float do_point_light(vec3 normal, vec3 light_pos, vec3 frag_pos, float range)
{
    vec3 light_vector = light_pos - frag_pos;
    float light_distance = length(light_vector);

    float falloff = 1.0 - pow(clamp(light_distance / range, 0.0, 1.0), 2);

    return falloff * incidence_factor(normal, normalize(light_vector));
}

float do_directional_light(vec3 normal, vec3 dir)
{
    return incidence_factor(normal, -dir);
}

vec3 calculate_light_factor()
{
    if (u_LightType == LIGHT_TYPE_AMBIENT) {
        return u_LightDiffuse.rgb;
    }

    vec3 normal = decode_normal(texture(u_GbufferNormal, v_TexCoords).xy);
    float amount = 0.0;

    switch (u_LightType) {
    case LIGHT_TYPE_POINT: {
        float depth = texture(u_GbufferDepth, v_TexCoords).r;
        vec3 view_pos = unproject(vec3(v_TexCoords, depth) * 2.0 - 1.0);

        amount = do_point_light(
            normal, u_LightPosition.xyz, view_pos, u_LightPosition.w);
        break;
    }
    case LIGHT_TYPE_DIRECTIONAL: {
        amount = do_directional_light(normal, u_LightPosition.xyz);
        break;
    }
    }

    return u_LightDiffuse * amount;
}

void main()
{
    vec4 albedo = texture(u_GbufferAlbedo, v_TexCoords);

    if (albedo.a == 0.0) {
        discard;
    }

    vec3 diffuse = albedo.rgb * calculate_light_factor();

    o_Color = vec4(diffuse, albedo.a);
}
