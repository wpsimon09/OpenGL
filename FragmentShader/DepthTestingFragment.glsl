#version 330 core

out vec4 FragColor;

in VS_OUT{
	in vec2 texCoords;
}fs_in;

uniform sampler2D texture_diffuse1;

void main(){
	FragColor = texture(texture_diffuse1, fs_in.texCoords);
}