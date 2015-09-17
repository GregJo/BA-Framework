#version 420

layout (binding=0) uniform sampler2D referenceOITTextureSampler;
layout (binding=1) uniform sampler2D comparisonOITTextureSampler;

layout(location = 0) out vec4 FragColor;

in vec2 UV;
in vec4 Position;

uniform sampler2D sampler;

void main() 
{
	vec4 referenceImage = texture( referenceOITTextureSampler, UV );
	vec4 comparisonImage = texture( comparisonOITTextureSampler, UV );
	vec4 differenceImage1 = (referenceImage-comparisonImage); //abs((referenceTexture-comparisonTexture));
	vec4 differenceImage2 = (comparisonImage-referenceImage);

	float grayResult1 = dot(differenceImage1.rgb, vec3(0.299, 0.587, 0.114));
	float grayResult2 = dot(differenceImage2.rgb, vec3(0.299, 0.587, 0.114));

	vec4 resultImage = vec4(0.0,0.0,0.0,1.0);

	if(grayResult1 > 0)
	{
		resultImage += vec4(grayResult1,0.0,0.0,0.0);
	}
	if(grayResult1 < 0)
	{
		resultImage += vec4(0.0,0.0,grayResult1,0.0);
	}
	if(grayResult2 > 0)
	{
		resultImage += vec4(0.0,grayResult2,0.0,0.0);
	}
	if(grayResult2 < 0)
	{
		resultImage += vec4(0.0,0.0,grayResult2,0.0);
	}

	FragColor = resultImage;
    //FragColor = differenceImage2;
	//FragColor = pow(resultImage,vec4(200,200,100,1));
	//FragColor = referenceImage;
	//FragColor = comparisonImage;
}