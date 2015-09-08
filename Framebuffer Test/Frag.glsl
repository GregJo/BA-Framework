#version 420

layout (std140) uniform Material 
{
    vec4 diffuse;
    vec4 ambient;
    vec4 specular;
    vec4 emissive;
    float shininess;
    //int texCount;
};

in vec2 UV;
in vec4 Position;
in vec3 Normal;
in vec4 eyeLightPosition;
in vec4 eyeCamPosition;

layout (binding=0) uniform sampler2D diffuse_Sampler;
layout (binding=1) uniform sampler2D depth_Sampler;
uniform vec3 LightIntensity = vec3(1.0f,1.0f,1.0f);
uniform vec3 Kd = vec3(1.0f,1.0f,1.0f); // Diffuse reflectivity
uniform vec3 Ka = vec3(0.0f,0.0f,0.0f); // Ambient reflectivity
uniform vec3 Ks = vec3(1.0f,0.5f,0.0f); // Specular reflectivity
uniform float Shininess = 200.0f; // Specular shininess factor

layout( location = 0 ) out vec4 FragColor;

uniform vec2 screenSize = vec2(1200.0,800.0);

// Specular correction factor
float C_NORMALIZATION = 1.0 / 30.0;

vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / screenSize;
}

vec3 ads()
{
	vec3 n = normalize( Normal );
	vec3 s = normalize( (eyeLightPosition - Position).xyz );
	vec3 v = normalize( vec3(eyeCamPosition - Position) );
	vec3 h = normalize( v + s );
	
	return LightIntensity * ( Ka + Kd * max( dot(s, n), 0.0 ));// + Ks * pow(max(dot(h,n),0.0), Shininess ) * (Shininess + 8.0) * C_NORMALIZATION );//LightIntensity * ( Ka + Kd * max( dot(s, n), 0.0 ) + Ks * pow(max(dot(h,n),0.0), Shininess ) * (Shininess + 8.0) * C_NORMALIZATION );
}

void main() 
{
	vec4 texColor = texture( diffuse_Sampler, UV );
	vec4 depthColor = texture( depth_Sampler, CalcTexCoord() );
	//FragColor = vec4(ads(),1.0) * texColor;
	FragColor = vec4(ads(),1.0) * texColor + 0.5*pow(depthColor,vec4(500,500,500,1));
	//FragColor = texColor;
	//FragColor = vec4(1.0,0.0,0.0,1.0)*vec4(ads(),1.0);
}