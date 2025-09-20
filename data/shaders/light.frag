#version 430 core

in vec3 normal;
in vec3 position;
in vec2 texcoord0;
in vec2 texcoord1;

uniform sampler2D tex0;
uniform sampler2D tex1;

out vec4 color;

void main() {
    color = texture(tex0, texcoord0);
}
