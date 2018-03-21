#version 330 core

//declaration of the buffer block
uniform p_block{
    int p[512];
};

in vec2 uv;
in vec2 off;

out vec3 color;

float perlin(float x, float y, int repeat);

float fade(float t);

int inc(int num, int repeat);

float lerp(float a, float b, float x);

float grad(int hash, float x, float y);


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
             value += perlin(p*freq)*amp;
             amp *= 0.5f;
             freq *= 2.0f;
         }
         return value;
     }

     //ridged multifractal
     float ridged_multifractal(vec2 p, float amp, float freq, int octave) {
         float l = 2.0f;
         float value = 0.0f;
         for(int i = 0; i < octave; i++) {
             vec2 v = p.xy * l;
             float n = perlin(p);
             n = abs(fract(n - 0.5f) -0.5f);
             n = n * n * amp;
             value += n * pow(freq, -1.0f);
             freq *= l;
         }

         return value*float(octave);
     }


void main() {
    float color_r = ridged_multifractal(uv, 1.9f, 5.0f, 8)-0.4f;
    float color_op = octave_perlin(uv, 0.7f, 5.0f, 16) - 0.6f;
    color = vec3((mix(color_r, color_op, 0.7f))/1.7f);
}

