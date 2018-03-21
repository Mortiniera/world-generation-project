#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D tex;
uniform sampler2D night;
uniform sampler2D sunset;
uniform float time;
uniform int day_time;

void main() {

    vec3 color_night = texture(night, uv).rgb;
    vec3 color_afternoon = texture(tex, uv).rgb;
    vec3 color_sunset = texture(sunset, uv).rgb;


    if(day_time == 0) {
        color = color_afternoon;
    }
    else if(day_time == 1) {
        color = color_sunset;
    }
    else if(day_time == 2){
        color = color_night;
    }
}
