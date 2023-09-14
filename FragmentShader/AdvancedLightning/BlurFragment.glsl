#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;

uniform bool horizontal;

uniform float weights[5] = float[] (0.227027, 0.1945946, 0.1216216,0.054054, 0.016216);

void main()
{
	vec2 texOffSet = 1/textureSize(image, 0); //calculate the size of single texelFetch
	vec3 result = texture(image, TexCoords).rgb; //grab the color for curent fragment

	if(horizontal)
	{
		for(int i = 0; i<5; ++i)
		{
			//x values to the right
			result += texture(image, TexCoords + vec2(texOffSet.x *i, 0.0)).rgb * weights[i];
			//x values to the left
			result += texture(image, TexCoords - vec2(texOffSet.x *i, 0.0)).rgb * weights[i];
		}
	}
	else 
	{
		for(int i = 0; i<5; ++i)
		{
			//y values up
			result += texture(image, TexCoords + vec2(texOffSet.y *i, 0.0)).rgb * weights[i];
			//y values down
			result += texture(image, TexCoords - vec2(texOffSet.y *i, 0.0)).rgb * weights[i];
		}
	}

	FragColor = vec4(result, 1.0);
}