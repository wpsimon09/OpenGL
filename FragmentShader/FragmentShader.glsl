#version 330

in vec3 ourColor;
in vec2 TexCoord;
out vec4 fragmentColor;

uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;

uniform vec3 objectColor;
uniform vec3 lightColor;

uniform float textureOpacity;

void main() {
	fragmentColor = vec4(lightColor * objectColor, 1.0);
}