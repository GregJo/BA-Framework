#version 420

in vec2 UV;

uniform sampler2D diffuse_Sampler;

layout( location = 0 ) out vec4 accumTextureFragColor;
layout( location = 1 ) out vec4 revealageTextureFragColor;

void main() 
{
	vec4 texColor = texture( diffuse_Sampler, UV );
	vec3 Ci = texColor.rgb*texColor.a;
	float ai = texColor.a; 
	accumTextureFragColor = vec4(Ci,ai);
	revealageTextureFragColor = vec4(ai);
}