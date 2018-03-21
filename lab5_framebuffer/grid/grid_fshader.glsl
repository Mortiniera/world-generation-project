#version 330
uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;
uniform bool mirror;

uniform sampler2D tex;
uniform sampler2D grass_tex;
uniform sampler2D rock_tex;
uniform sampler2D sand_tex;
uniform sampler2D snow_tex;


in vec3 light_dir;
in vec3 view_dir;
in vec3 normal_mv;
in vec4 v_point;
in vec2 uv;

out vec3 color;


void main() {
    //implement phong shading
   vec3 phong = vec3(0.0, 0.0, 0.0);

   //diffuse term of the phong shading
   vec3 n = normalize(normal_mv);
   vec3 l = normalize(light_dir);
   float lambert = dot(-n, l);
   if(lambert > 0.0) {
       phong += kd*lambert*Ld;
   }

   //apply other textures to color the grid
   vec3 grass = texture(grass_tex, uv).rgb;
   vec3 rock = texture(rock_tex, uv).rgb;
   vec3 sand = texture(sand_tex, uv).rgb;
   vec3 snow = texture(snow_tex, uv).rgb;

   float height = normalize(v_point).y;
   float normal = normalize(normal_mv).z;
   //grass height color gradient
   vec3 grid_color = mix(grass, rock, height*5);

   //rock
   if (normal < 0.35) {
       grid_color = mix(rock, grid_color, 1.0f + (normal - 0.25f)*1.1);
   }

   //sand
   if(height < 0.1f) {
       grid_color = mix(sand, grid_color, clamp(height*50, 0, 1));
   }
   float snow_h = 0.2f;
   //snow
   if(height > snow_h) {
       grid_color = mix(grid_color, snow, clamp((height - snow_h)*10, 0, 1));
   }

   //underwater color
   if(height < 0) {
       if (mirror) {
           discard;
       } else {
           grid_color = mix(vec3(0.0f, 0.5f, 0.45f), sand, height*50);
       }
   }

   color = mix(phong, grid_color, 0.7);


}
