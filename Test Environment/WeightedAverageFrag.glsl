#version 420

layout (binding = 0) uniform sampler2D accumTextureSampler;
layout (binding = 1) uniform sampler2D countTextureSampler;

layout( location = 0 ) out vec4 FragColor;

void main() 
{
	vec4 accum = texelFetch(accumTextureSampler,ivec2(gl_FragCoord.xy),0);
	float n = max(1.0,texelFetch(countTextureSampler,ivec2(gl_FragCoord.xy),0).r);

	FragColor = vec4((accum.rgb / max(accum.a,0.0001)),pow(max(0.0,(1.0-(accum.a/n))),n));
}