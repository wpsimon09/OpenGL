#version 330 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

uniform sampler2D lightTexture;

uniform vec3 lightColor;

in vec2 TextureCoords;

void main() {

	vec4 texColor = texture(lightTexture, TextureCoords);
	if(texColor.a < 0.1)
		discard;

	FragColor = vec4(lightColor, 1.0f);

	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
		if(brightness > 1.0)
			FragColor= vec4(FragColor.rgb, 1.0);
		else
			FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}