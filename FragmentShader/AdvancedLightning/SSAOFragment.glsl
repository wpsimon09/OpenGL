#version 330 core

in vec2 TexCoord;

//note the datatype as we onyl have the GL_RED option as the texture color
out float FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];
uniform mat4 projection;

const vec2 noiseScale = vec2(1000.0/4.0, 800.0/4.0);

int kernelSize = 64;
float radius = 0.5;

void main()
{
	vec3 fragPos = texture(gPosition, TexCoord).xyz;
	vec3 normal = normalize(texture(gNormal, TexCoord).rgb);

	// we have to multiply the texture coordinates by noise scale as its set to GL_REPEAT
	vec3 randomVec = normalize(texture(texNoise, TexCoord* noiseScale).xyz);
	
	//-----------------------
	// TBN MATRIX CALCULATION
	//-----------------------
	// no need for per vertex tangent and bitangent values	
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	//----------------------
	// CALCUALTION OCCULSION
	//---------------------
	float occlusion = 0.0;
	for(int i = 0; i < kernelSize; ++i)
	{
		vec3 samplePos = TBN * samples[i]; //from tangent space to view-space
		samplePos = fragPos + samplePos * radius; //get the sample position offseted by radius
	
		vec4 offset = vec4(samplePos, 1.0);
		offset = projection * offset; // from view-space to screen (clip) space
		offset.xyz /= offset.w; //perspective dividion
		offset.xyz = offset.xyz * 0.5 + 0.5; //transform to range 0.0 - 1.0

		//get the depth value of the sample
		float sampleDepth = texture(gPosition, offset.xy).z;

		float rangeCheck = smoothstep(0.0, 1.0, radius)/abs(fragPos.z - sampleDepth);

		occlusion += (sampleDepth >= samplePos.z + 0.025 ? 1.0 : 0.0) * rangeCheck;
	}
	occlusion = 1.0 - (occlusion / kernelSize);
	FragColor = occlusion;
}