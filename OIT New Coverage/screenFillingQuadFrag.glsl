#version 400 

layout(location = 0) out vec4 FragColor;

in vec2 UV;
in vec4 Position;

uniform sampler2D sampler;

void main() 
{
	vec4 tex = texture( sampler, UV );
	FragColor = tex;
	//FragColor = pow(tex,vec4(200,200,100,1));
}