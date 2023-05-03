#version 330

in vec3 ourColor;
in vec2 TexCoord;
out vec4 fragmentColor;

uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;

uniform float textureOpacity;

void main() {
	fragmentColor = vec4(1.0f, 0.5f, 0.85f, 1.0f);
	//mix(texture(ourTexture, TexCoord), texture(ourTexture2, vec2(-TexCoord.x, TexCoord.y)), textureOpacity);
}