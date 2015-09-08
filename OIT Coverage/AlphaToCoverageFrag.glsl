#version 400

layout (std140) uniform Material 
{
    vec4 diffuse;
    vec4 ambient;
    vec4 specular;
    vec4 emissive;
    float shininess;
    //int texCount;
} Mat;

in vec2 UV;
in vec4 Position;
in vec3 Normal;
in vec4 eyeLightPosition;
in vec4 eyeCamPosition;

uniform sampler2D diffuse_Sampler;
uniform vec3 LightIntensity = vec3(1.0f,1.0f,1.0f);
uniform vec3 ambientLightDir = normalize(vec3(1.0f,-1.0f,-1.0f));
uniform vec3 Kd = vec3(1.0f,1.0f,1.0f); // Diffuse reflectivity
uniform vec3 Ka = vec3(0.0f,0.0f,0.0f); // Ambient reflectivity
uniform vec3 Ks = vec3(1.0f,0.5f,0.0f); // Specular reflectivity
uniform float Shininess = 200.0f; // Specular shininess factor

// Alpha to coverage paramters (assuming that I dont use any of the alpha to coverage OpenGL features)
// coverage mask and its sqrt(size) for multisampling (coverage mask might need to be a whole coverage texture)
// last pass texture for multi sampling

layout( location = 0 ) out vec4 FragColor;

// Specular correction factor
float C_NORMALIZATION = 1.0 / 30.0;

vec3 ads()
{
	vec3 n = normalize( Normal );
	vec3 s = normalize( (eyeLightPosition - Position).xyz );
	vec3 v = normalize( vec3(eyeCamPosition - Position) );
	vec3 h = normalize( v + s );
	
	return  (Mat.ambient.xyz * dot(-ambientLightDir,n)) + (LightIntensity * (Mat.diffuse.xyz * max( dot(s, n), 0.0 ) + Mat.specular.xyz * pow(max(dot(h,n),0.0), Mat.shininess ) * (Mat.shininess + 8.0) * C_NORMALIZATION ));
	//LightIntensity * ( Ka + Kd * max( dot(s, n), 0.0 ) + Ks * pow(max(dot(h,n),0.0), Shininess ) * (Shininess + 8.0) * C_NORMALIZATION );
}

void main() 
{
	vec4 texColor = texture( diffuse_Sampler, UV );
	FragColor = vec4(ads(),texColor.a) * texColor;
	//FragColor = texColor;
	//FragColor = vec4(1.0,0.0,0.0,1.0)*vec4(ads(),1.0);
}