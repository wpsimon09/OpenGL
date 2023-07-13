#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

float near = 0.1f;
float far = 100.0f;

uniform sampler2D texture1;

float LinearizedDepth(float depth)
{
	float z = depth * 2.0 - 1.0; // back to NDC
	return (2.0 * near * far) / (far + near - z * (far - near));
}


void main(){
	FragColor = texture(texture1, TexCoords);
}