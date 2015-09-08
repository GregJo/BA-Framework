#version 400 

layout (std140) uniform Material 
{
    vec4 diffuse;
    vec4 ambient;
    vec4 specular;
    vec4 emissive;
    float shininess;
    //int texCount;
};

in vec2 texCoord; 
in vec3 normal; 
in vec3 worldPos; 
in float depth; 

layout (location = 0) out vec3 worldPosOut; 
layout (location = 1) out vec3 diffuseOut; 
layout (location = 2) out vec3 normalOut; 
layout (location = 3) out vec3 texCoordOut; 
layout (location = 4) out vec3 depthOut; 

uniform sampler2D colorMap; 

void main() 
{ 
    worldPosOut = worldPos; 
    diffuseOut = texture(colorMap, texCoord).xyz; 
    normalOut = normalize(normal).xyz; 
    texCoordOut = vec3(texCoord, 0.0);
	depthOut = vec3(depth,depth,1); 
}