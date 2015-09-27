#version 420

in vec2 UV;

layout (binding=0) uniform sampler2D diffuseTextureSampler;
layout (binding=1) uniform sampler2D backgroundTextureSampler;
uniform vec2 windowSize = vec2(1200,800);

layout( location = 0 ) out vec4 FragColor;

vec2 CalcTexCoord()
{
	//eturn UV/screenSize;
    return gl_FragCoord.xy / windowSize;
}

void main() 
{
	vec3 C0 = texture( backgroundTextureSampler, CalcTexCoord() ).rgb;
	//vec3 C0 = texelFetch( backgroundTextureSampler, ivec2(gl_FragCoord.xy), 0 ).rgb;
	//float a0 = texture( backgroundTextureSampler, CalcTexCoord() ).a;
	//C0=C0*a0;
	vec3 Ci = texture( diffuseTextureSampler, UV ).rgb;
	float ai = texture( diffuseTextureSampler, UV ).a;
	//float ai = 0.3;
	Ci = Ci*ai;
	
	vec4 result = vec4(Ci-ai*C0,1.0);

	//result = vec4(clamp(result.r,-1.0,1.0), clamp(result.g,-1.0,1.0), clamp(result.b,-1.0,1.0), 1.0);

	FragColor = result;
}