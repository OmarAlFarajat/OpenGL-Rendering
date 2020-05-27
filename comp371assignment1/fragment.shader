// Custom code for the most part, but some referenced from http://opengl-tutorial.org/ and http://learnopengl.com/
#version 330 core

in vec2 UV;
in vec3 vColor; 
in vec3 vNormal; 
in vec3 FragPos; 
in vec4 ShadowCoord;

uniform mat4 Model;
uniform mat4 View; 

uniform vec3 CameraPosition;
uniform sampler2D myTextureSampler;
uniform sampler2DShadow shadowMap;

uniform bool FlagA;

layout(location = 0) out vec4 color;



void main()
{
	if (FlagA == true) {
	
		// PART A: Phong lighting model. Light positions and colors fixed directly in shader

		// LIGHT #1
		// attenuation
		float distance = length(vec3(10.0f, 15.0f, 5.0f) - FragPos);
		float attenuation = 1.0 / (1.0f + 0.05 * distance + 0.01 * (distance * distance));
		// Light direction
		vec3 lightDir = normalize(vec3(10.0f, 15.0f, 5.0f) - FragPos);
		// AMBIENT
		vec3 ambient = 0.25f * vec3(1.0f, 0.0f, 0.0f) * attenuation;
		// DIFFUSE
		vec3 vNormal = normalize(vNormal);
		float diff = max(dot(vNormal, lightDir), 0.0);
		vec3 diffuse = 1.0f * diff * vec3(1.0f, 0.0f, 0.0f) * attenuation;
		// SPECULAR
		vec3 reflectDir = reflect(-lightDir, vNormal);
		float spec = pow(max(dot(normalize(CameraPosition - FragPos), reflectDir), 0.0), 32);
		vec3 specular = 1.0f * spec * vec3(1.0f, 0.0f, 0.0f) * attenuation;

		// LIGHT #2
		// attenuation
		distance = length(vec3(-10.0f, 15.0f, 5.0f) - FragPos);
		attenuation = 1.0 / (1.0f + 0.05 * distance + 0.01 * (distance * distance));
		// Light direction
		lightDir = normalize(vec3(-10.0f, 15.0f, 5.0f) - FragPos);
		// AMBIENT
		ambient += 0.25f * vec3(0.0f, 1.0f, 0.0f) * attenuation;
		// DIFFUSE
		lightDir = normalize(vec3(-10.0f, 15.0f, 5.0f) - FragPos);
		diff = max(dot(vNormal, lightDir), 0.0);
		diffuse += 1.0f * diff * vec3(0.0f, 1.0f, 0.0f) * attenuation;
		// SPECULAR
		reflectDir = reflect(-lightDir, vNormal);
		spec = pow(max(dot(normalize(CameraPosition - FragPos), reflectDir), 0.0), 32);
		specular += 1.0f * spec * vec3(0.0f, 1.0f, 0.0f) * attenuation;

		// LIGHT #3
		// attenuation
		distance = length(vec3(0.0f, 15.0f, 5.0f) - FragPos);
		attenuation = 1.0 / (1.0f + 0.05 * distance + 0.01 * (distance * distance));
		// Light direction
		lightDir = normalize(vec3(0.0f, 15.0f, 5.0f) - FragPos);
		// AMBIENT
		ambient += 0.25f * vec3(0.0f, 0.0f, 1.0f) * attenuation;
		// DIFFUSE
		lightDir = normalize(vec3(0.0f, 15.0f, 5.0f) - FragPos);
		diff = max(dot(vNormal, lightDir), 0.0);
		diffuse += 1.0f * diff * vec3(0.0f, 0.0f, 1.0f) * attenuation;
		// SPECULAR
		reflectDir = reflect(-lightDir, vNormal);
		spec = pow(max(dot(normalize(CameraPosition - FragPos), reflectDir), 0.0), 32);
		specular += 1.0f * spec * vec3(0.0f, 0.0f, 1.0f) * attenuation;

		// LIGHT #4
		// attenuation
		distance = length(vec3(0.0f, 0.0f, 25.0f) - FragPos);
		attenuation = 1.0 / (1.0f + 0.05 * distance + 0.01 * (distance * distance));
		// Light direction
		lightDir = normalize(vec3(0.0f, 0.0f, 25.0f) - FragPos);
		// AMBIENT
		ambient += 0.25f * vec3(1.0f, 1.0f, 1.0f) * attenuation;
		// DIFFUSE

		lightDir = normalize(vec3(0.0f, 0.0f, 25.0f) - FragPos);
		diff = max(dot(vNormal, lightDir), 0.0);
		diffuse += 1.0f * diff * vec3(1.0f, 1.0f, 1.0f) * attenuation;
		// SPECULAR
		reflectDir = reflect(-lightDir, vNormal);
		spec = pow(max(dot(normalize(CameraPosition - FragPos), reflectDir), 0.0), 32);
		specular += 1.0f * spec * vec3(1.0f, 1.0f, 1.0f) * attenuation;

		// Cumulative and contributive result
		vec3 result = (ambient + diffuse + specular) * vColor;
		color = vec4(result* texture(myTextureSampler, UV).rgb,1.0f);
	}
	else {
		// PART B: Lambert lighting model implemented with sampler2DShadow.
		// Sourced from Newcastle University, UK: https://research.ncl.ac.uk/game/mastersdegree/graphicsforgames/shadowmapping/Tutorial%2014%20-%20Shadow%20Mapping.pdf		vec4 diffuse = texture2D (myTextureSampler, UV);
		vec3 incident = normalize(vec3(0.0f, 20.0f, 10.0f) - FragPos);
		float lambert = max(0.0, dot(incident, vNormal));
		float dist = length(vec3(0.0f, 20.0f, 10.0f) - FragPos);
		float atten = 1.0 - clamp(dist / 40.0f, 0.0, 1.0);	
		vec3 viewDir = normalize(CameraPosition - FragPos);
		vec3 halfDir = normalize(incident + viewDir);	
		float rFactor = max(0.0, dot(halfDir, vNormal)); 
		float sFactor = pow(rFactor, 33.0);
		float shadow = 1.0; 	
		if (ShadowCoord.w > 0.0) { 
			shadow = textureProj(shadowMap, ShadowCoord);
			lambert *= shadow; 
			vec3 colour = (diffuse.rgb * vec3(1.0f,1.0f,1.0f));
			colour += (vec3(1.0f, 1.0f, 1.0f) * sFactor) * 0.33;
			color = vec4(colour * atten * lambert, diffuse.a);
			color.rgb += (diffuse.rgb * vec3(1.0f, 1.0f, 1.0f)) * 0.1;	
		}
	}
}