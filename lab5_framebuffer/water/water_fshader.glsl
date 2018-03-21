#version 330

in vec2 uv;
in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;

out vec3 color;

uniform sampler2D tex_mirror;
uniform sampler2D water_tex;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;
uniform float tex_width;
uniform float tex_height;

void main() {

    //Mirror texture
    vec2 dim = textureSize(tex_mirror, 0);
    vec2 new_uv = vec2(gl_FragCoord.x/dim.x, 1 - gl_FragCoord.y/dim.y); //taille fenetre
    vec3 color_from_mirror = texture(tex_mirror, new_uv).rgb;

    //Phong shading
    vec3 phong = vec3(64.0/255.0, 164.0/255.0, 223.0/255.0);

    phong += ka * La;
    vec3 n = normalize(normal_mv);
    vec3 l = normalize(light_dir);
    float lambert = dot(-n, l);
    if(lambert > 0.0) {
        phong += kd*lambert*Ld;
        vec3 v = normalize(view_dir);
        vec3 r = reflect(-l, n);
        phong += ks*pow(max(0.0, dot(r, v)), alpha)*Ls;
    }

    color = vec3(64.0/255.0, 164.0/255.0, 223.0/255.0);
    color = mix(color, phong, 0.7);
    if(gl_FrontFacing) {
        color = mix(color, color_from_mirror, 0.7f);
    }
}
