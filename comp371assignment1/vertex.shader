// Custom code for the most part, but some referenced from http://opengl-tutorial.org/ and http://learnopengl.com/
#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

out vec3 vColor; 
out vec3 vNormal; 
out vec3 FragPos; 
out vec2 UV;

out vec4 ShadowCoord;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform mat4 DepthBiasMVP;

uniform vec3 CameraPosition;

uniform bool FlagA;

void main()
{
	gl_Position = Projection * View * Model * vec4(vertexPosition_modelspace, 1.0f);
	UV = vertexUV; 
	vNormal = mat3(transpose(inverse(Model))) * vertexNormal_modelspace;
	vColor = vec3(1.0f, 1.0f, 1.0f);
	FragPos = vec3(Model * vec4(vertexPosition_modelspace, 1.0f));
	ShadowCoord = DepthBiasMVP * vec4(vertexPosition_modelspace, 1);

}