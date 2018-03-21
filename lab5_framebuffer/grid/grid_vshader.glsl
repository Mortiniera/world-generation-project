#version 330

#define M_PI 3.14159265358979323846
in vec2 position;
in vec3 vnormal;
in float dim;

out vec2 uv;
out vec3 normal_mv;
out vec3 light_dir;
out vec3 view_dir;
out vec4 v_point;

uniform mat4 MVP;
uniform mat4 MV;
uniform sampler2D tex;
uniform vec3 light_pos;


void main() {

    uv = (position + vec2(1.0, 1.0)) * 0.5;

    float height = 0.0;
    height = texture(tex, uv).r;

    float tex_width = textureSize(tex, 0).x;
    float tex_height = textureSize(tex, 0).y;
    float right_height = textureOffset(tex, uv ,ivec2(1, 0)).r;
    float left_height = textureOffset(tex, uv ,ivec2(-1, 0)).r;
    float top_height = textureOffset(tex, uv ,ivec2(0, 1)).r;
    float bottom_height = textureOffset(tex, uv, ivec2(0, -1)).r;

    vec3 dx = vec3(2.0f/tex_width, right_height - left_height, 0.0);
    vec3 dy = vec3(0.0, top_height - bottom_height, 2.0f/tex_height);
    vec3 n = normalize(cross(dy, dx));
    normal_mv = n;

    vec3 pos_3d = vec3(position.x, height, -position.y);
    v_point = vec4(pos_3d, 1.0);
    gl_Position = MVP * vec4(pos_3d, 1.0);
    vec4 vpoint_mv = MV * gl_Position;

    /* We compute the light direction light_dir. */
    light_dir = normalize(light_pos - vpoint_mv.xyz);

    /* We compute the view direction view_dir */
    view_dir = normalize(-vpoint_mv.xyz);
}
