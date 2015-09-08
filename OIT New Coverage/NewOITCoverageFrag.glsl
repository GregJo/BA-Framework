#version 420

layout (binding = 0) uniform sampler2D accumTextureSampler;
layout (binding = 1) uniform sampler2D revealageTextureSampler;

layout( location = 0 ) out vec4 FragColor;

void main() 
{
	vec4 accum = texelFetch(accumTextureSampler,ivec2(gl_FragCoord.xy),0);
	float r = texelFetch(revealageTextureSampler,ivec2(gl_FragCoord.xy),0).r;

	FragColor = vec4(accum.rgb / clamp(accum.a, 1e-4, 5e4),r);
	//FragColor = vec4(1.0,0.75,0.0,1.0);
}