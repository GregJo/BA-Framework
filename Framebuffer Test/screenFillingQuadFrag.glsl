#version 400 

layout(location = 0) out vec4 FragColor;

in vec2 UV;
in vec4 Position;

uniform sampler2D sampler;

void main() 
{
	vec4 tex = texture( sampler, UV );
	FragColor = tex;
	//FragColor = pow(tex,vec4(200,200,100,0));
	//FragColor = clamp(2*tex,vec4(0,0,0,0),vec4(1,1,1,1));
	//if(tex.r==1.0)
	//{
	//	FragColor= vec4(0.4f,0.6f,0.94f,1.0f);
	//}
}