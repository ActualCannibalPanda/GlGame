#version 430 core
in vec3 normal;
in vec3 position;
in vec2 texcoord0;
in vec2 texcoord1;

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform vec3 sun_position; 
uniform vec3 sun_color; 

out vec4 color;

void main() {
/* 	float lum = max(dot(normal, normalize(sun_position)), 0.0); */
	color = texture(tex0, texcoord0);/* * vec4((0.3 + 0.7 * lum) * sun_color, 1.0); */
}
