#version 330

in vec2 vpoint;

out vec2 uv;
out vec3 normal_mv;
out vec3 light_dir;
out vec3 view_dir;

uniform mat4 MVP;
uniform mat4 MV;

uniform vec3 light_pos;
uniform float tex_width;
uniform float tex_height;
uniform float time;

//Smooth interpolation function
float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

//Random normal vector, found in http://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
float rnd(vec2 p){
    return fract(sin(dot(p.xy ,vec2(12.9898,78.233))) * 43758.5453);
}


//perlin noise in 2D
float perlin(vec2 p) {

    vec2 x0y0 = floor(p);


    //dot products to get scalar values for the corners
    float s = rnd(x0y0);
    float t = rnd(x0y0 + vec2(1.0, 0.0));
    float u = rnd(x0y0 + vec2(0.0, 1.0));
    float v = rnd(x0y0 + vec2(1.0, 1.0));

    vec2 f = vec2(fade(fract(p.x)), fade(fract(p.y)));
    float st = mix(s, t, f.x);
    float uv = mix(u, v, f.x);

    return mix(st, uv, f.y);

}

//fractal brownian motion
float octave_perlin(vec2 p, float amp, float freq, int octave) {

    float value = 0.0f;
    for(int i = 0; i < octave; i++) {
        value += perlin(p*freq + time/3.0)*amp;
        amp *= 0.5f;
        freq *= 2.0f;
    }
    return value;
}

float waves(vec2 p) {
    return (octave_perlin(p, 0.2f, 400.0f, 1)-0.2f)/10.0;
    //return octave_perlin(uv, 0.5f, 5.0f, 4)/100.0;
}

void main() {

    uv = (vpoint + vec2(1.0, 1.0)) * 0.5;
    vec3 pos_3d = vec3(vpoint.x, /*sin(uv.x * time)/1000.0*/0, -vpoint.y);
    gl_Position = MVP * vec4(pos_3d, 1.0);
    vec4 vpoint_mv = MV * gl_Position;

    float dx = waves(vec2(uv.x+1.0f/tex_width, uv.y)) - waves(vec2(uv.x - 1.0f/tex_width, uv.y));
    float dy = waves(vec2(uv.x, uv.y + 1.0/tex_height)) - waves(vec2(uv.x, uv.y - 1.0/tex_height));
    vec3 A = vec3(2.0/tex_width, dx, 0.0f);
    vec3 B = vec3(0.0f, dy, 2.0/tex_height);
    normal_mv = normalize(cross(B,A));

    /* We compute the light direction light_dir. */
    light_dir = normalize(light_pos - vpoint_mv.xyz);

    /* We compute the view direction view_dir */
    view_dir = normalize(-vpoint_mv.xyz);
}
