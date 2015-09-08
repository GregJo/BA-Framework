#version 420

layout (binding=0) uniform sampler2D texSampler;
layout (binding=1) uniform sampler2D depthSampler;

uniform vec2 screenSize = vec2(1200.0,800.0);

// Might not be necessary!
layout( location = 0 ) out vec4 FragColor;
//layout( location = 1 ) out vec4 FragDepth;

in vec2 UV;

vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / screenSize;
}

void main() 
{
	vec2 texCoord = CalcTexCoord();
	vec4 firstPassDepth = texture( depthSampler, texCoord );
	vec4 tex = texture( texSampler, UV );
	if(gl_FragCoord.z <= firstPassDepth.z)
	{
		discard;
	}
	else
	{
		FragColor = tex;
	}
}