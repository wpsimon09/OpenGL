#version 330

in vec3 ourColor;
in vec2 TexCoord;
out vec4 fragmentColor;

uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;

uniform float textureOpacity;

void main() {
	fragmentColor = mix(texture(ourTexture, TexCoord), 
					texture(ourTexture2, vec2(-TexCoord.x, TexCoord.y)), textureOpacity);
}