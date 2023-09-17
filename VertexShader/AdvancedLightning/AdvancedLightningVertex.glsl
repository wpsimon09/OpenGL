#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangetn;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in mat4 instanceModel;

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
    // transform the vectors to the world space 
    vec3 T = normalize(vec3(instanceModel * vec4(aTangetn, 0.0)));
    vec3 B = normalize(vec3(instanceModel * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(instanceModel * vec4(aNormal, 0.0)));
    gl_Position = projection * view * instanceModel * vec4(aPos, 1.0);
    
    // create the TBN matrix
    mat3 TBN = transpose(mat3(T,B,N));


    //TODO: check if the object has normal mapping if it has one calculate everything using tangent space and use normal maps for normal vectors
    //if object does not have normal send data in the wold space and use normal vectors passed in vertex attribute instead 
    vs_out.FragPos =  vec3(instanceModel * vec4(aPos, 1.0));
    vs_out.TexCoords = aTexCoords;
    vs_out.Normal = transpose(inverse(mat3(instanceModel))) * aNormal;
    vs_out.FragPosLight = lightMatrix * vec4(vs_out.FragPos ,1.0);
    vs_out.hasNormalMap = hasNormalMap;
    
    if(hasNormalMap == 1.0)
    {
        vs_out.TangentLightPos = TBN * lightPos;
        vs_out.TangentViewPos = TBN * viewPos;
        vs_out.TangentFragPos = TBN * vec3(instanceModel * vec4(aPos, 0.0));
    }
    else {
        vs_out.TangentLightPos = lightPos;
        vs_out.TangentViewPos = viewPos;
        vs_out.TangentFragPos = vec3(instanceModel * vec4(aPos, 0.0));
    }
}
