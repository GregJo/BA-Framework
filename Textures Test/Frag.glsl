#version 400

in vec2 UV;

uniform sampler2D diffuse_Sampler;

layout( location = 0 ) out vec4 FragColor;

void main() 
{
	vec4 texColor = texture( diffuse_Sampler, UV );
	FragColor = texColor;
	FragColor = vec4(1.0,0.0,0.0,1.0);
	//FragColor = vec4(0.4f,0.6f,0.94f,1.0f);
}