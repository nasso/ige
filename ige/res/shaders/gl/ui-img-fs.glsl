#version 410 core

uniform vec4 u_Tint;
uniform sampler2D u_Texture;
uniform vec4 u_TextureBorders; // border location on the texture
uniform vec4 u_Borders; // border location on screen
uniform vec2 u_RepeatCount;

in vec2 v_TexCoords;

out vec4 o_Color;

float map(float x, float orig_min, float orig_max, float new_min, float new_max)
{
    return mix(new_min, new_max, (x - orig_min) / (orig_max - orig_min));
}

float map_axis(float x, vec2 texture_border, vec2 border, float repeat_count)
{
    if (x < border.x) {
        return map(x, 0.0, border.x, 0.0, texture_border.x);
    } else if (x > border.y) {
        return map(x, border.y, 1.0, texture_border.y, 1.0);
    } else {
        float tile_size = texture_border.y - texture_border.x;
        float mapped
            = map(x, border.x, border.y, 0.0, tile_size * repeat_count);

        return texture_border.x + mod(mapped, tile_size);
    }
}

void main()
{
    vec2 uv = v_TexCoords;
    uv.x = map_axis(uv.x, u_TextureBorders.xz, u_Borders.xz, u_RepeatCount.x);
    uv.y = map_axis(uv.y, u_TextureBorders.yw, u_Borders.yw, u_RepeatCount.y);

    o_Color = u_Tint * texture(u_Texture, uv);
}
