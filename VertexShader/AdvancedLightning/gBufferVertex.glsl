#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangetn;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in mat4 instancedModel;

uniform mat4 projection;
uniform mat4 view;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform mat4 lightMatrix;

uniform float hasNormalMap;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLight;
    vec3 TangentViewPos;
    vec3 TangentLightPos;
    vec3 TangentFragPos;

    float hasNormalMap;
}vs_out;

void main()
{
    
    mat4 model = instancedModel;
    mat3 normalMatrix = transpose(inverse(mat3(model)));

    // transform the vectors to the world space 
    vec3 T = normalize(normalMatrix * aTangetn);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    // create the TBN matrix
    mat3 TBN = transpose(mat3(T,B,N));

    vs_out.FragPos =  vec3(model * vec4(aPos, 1.0));
    vs_out.TexCoords = aTexCoords;
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.FragPosLight = lightMatrix * vec4(vs_out.FragPos ,1.0);
    vs_out.hasNormalMap = hasNormalMap;
    
    if(hasNormalMap == 1.0)
    {
        vs_out.TangentLightPos = TBN * lightPos;
        vs_out.TangentViewPos = TBN * viewPos;
        vs_out.TangentFragPos = TBN * vec3(instancedModel * vec4(aPos, 0.0));
    }
    else {
        vs_out.TangentLightPos = lightPos;
        vs_out.TangentViewPos = viewPos;
        vs_out.TangentFragPos = vec3(model * vec4(aPos, 0.0));
    }

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
