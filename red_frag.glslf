#version 150 core

in vec3 frag_color;
in vec2 frag_texcoord;

out vec4 outColor;

uniform sampler2D tex;

void main() {
    outColor = texture(tex, frag_texcoord) * vec4(frag_color, 1.0);
}
