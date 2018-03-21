#version 330

in vec3 vpoint;
in vec2 vtexcoord;
in vec2 offset;

out vec2 uv;
out vec2 off;

void main() {
    gl_Position = vec4(vpoint, 1.0);
    uv = vtexcoord;
    off = offset;
}
