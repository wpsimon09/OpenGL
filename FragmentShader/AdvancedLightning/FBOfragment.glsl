#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform sampler2D blurred;

uniform float exposure = 1.0;
void main()
{
	vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
	vec3 bluredColor = texture(blurred, TexCoords).rgb;

	hdrColor += bluredColor;

	// exposure tone mapping
	vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);

	FragColor = vec4(mapped, 1.0);
}