#version 330 core

uniform sampler2D lightTexture;

in vec2 TextureCoords;

out vec4 FragmentColor;

void main() {

	vec4 texColor = texture(lightTexture, TextureCoords);
	if(texColor.a < 0.1)
		discard;

	FragmentColor = texColor;
}