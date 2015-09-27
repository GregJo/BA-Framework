#version 420

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec2 in_UV;
//layout(location = 2) in vec3 in_Normal;

uniform mat4 VPMatrix;
uniform float scaleCoeff = 1.0;
uniform vec3 worldspaceParticleCenter = vec3(0,0,0);//vec3(0,0,0);
uniform vec3 worldspaceCameraRightVector = vec3(1.0,0,0);
uniform vec3 worldspaceCameraUpVector = vec3(0,1.0,0);

mat4 scaleMatrix = mat4(scaleCoeff,0.0,0.0,0.0,
					0.0,scaleCoeff,0.0,0.0,
					0.0,0.0,scaleCoeff,0.0,
					0.0,0.0,0.0,1.0);

out vec2 UV;

void main()
{
	UV = in_UV;

	vec3 worldspaceVertexPosition = (scaleMatrix * vec4(in_Position,1.0f)).xyz;

	vec3 scaledWorldspaceVertexPosition = worldspaceParticleCenter
									 + worldspaceCameraRightVector * worldspaceVertexPosition.x
									 + vec3(0,worldspaceVertexPosition.y,0);

	gl_Position = VPMatrix * vec4(scaledWorldspaceVertexPosition,1.0f);
}