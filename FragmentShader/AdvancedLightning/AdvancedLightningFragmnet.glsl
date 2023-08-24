#version 330 core

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
}fs_in;

out vec4 FragColor;

uniform sampler2D wood;
uniform samplerCube shadowMap;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform bool blinnModel;

uniform float far_plane;

uniform vec3 specularColor;

float closestDepthDebug;

float caclualteShadow(vec3 FragPos, float bias)
{
    //get the directional vector between light and fragment that is beeing rendered
    vec3 fragToLight = FragPos - lightPos;

    //use the directional vector to sample from the cube map and retrieve the depth value
    float closestDepth = texture(shadowMap, fragToLight).r;
    //right now it is in the range [0, 1] we want it to be in the rangle of [0, far_plane]
    // so that we can compare it later on
    closestDepth *= far_plane;

    //retrieve the length between light and the current fragment 
    float curentDepth = length(fragToLight);

    float shadow = curentDepth - bias > closestDepth ? 1.0 : 0.0;

    closestDepthDebug = curentDepth;
    return shadow;

}

void main() 
{
    //----------
    // AMBIENT
    //----------
    vec3 ambient = vec3(texture(wood, fs_in.TexCoords)* 0.2);
    
    //-----------
    // DIFFUSE
    //-----------
    vec3 normal = normalize(-fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diffStrength = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = lightColor * diffStrength * vec3(texture(wood, fs_in.TexCoords));
    
    //---------
    //SPECULAR
    //---------
    vec3 specularColor = lightColor;
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    float specStrength = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    specStrength = pow(max(dot(normal, halfwayDir), 0.0),64.0);
    vec3 specular = specularColor * specStrength;
    
    //---------
    // SHADOWS
    //---------
    float bias = 0.05;

    float shadow = caclualteShadow(fs_in.FragPos, bias);
    //
    
    vec3 result = ambient + diffuse + specular;
    
    vec3 shadowResult = (ambient + (1.0 - shadow) * (diffuse + specular)) * texture(wood, fs_in.TexCoords).rgb;
    //-------------
    // FINAL RESULT
    //-------------    

    //vec4(vec3(closestDepthDebug / far_plane), 1.0);
    if(gl_FragCoord.y < 300)
        FragColor = vec4(closestDepthDebug, closestDepthDebug/2, 0, 1.0);
    else 
        FragColor = vec4(result, 1.0);
}