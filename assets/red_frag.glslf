#version 150 core

in vec3 frag_color;
in vec2 frag_texcoord;

out vec4 outColor;

uniform sampler2D tex1;
uniform sampler2D tex2;

void main() {
    vec4 col1 = texture(tex1, frag_texcoord);
    vec4 col2 = texture(tex2, frag_texcoord);
    outColor = mix(col1, col2, 0.5) * vec4(frag_color, 1.0);
}
