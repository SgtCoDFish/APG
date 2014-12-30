#version 150 core

in vec2 position;
in vec3 color;
in vec2 texcoord;

out vec2 frag_texcoord;
out vec3 frag_color;

void main() {
    frag_color = color;
    frag_texcoord = texcoord;
    gl_Position = vec4(position, 0.0, 1.0);
}
