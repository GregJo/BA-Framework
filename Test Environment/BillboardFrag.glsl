#version 420

layout(location = 0) out vec4 FragColor;

in vec2 UV;
in vec4 Position;

uniform sampler2D sampler;

void main() 
{
	vec4 tex = texture( sampler, UV );
	FragColor = tex;
}