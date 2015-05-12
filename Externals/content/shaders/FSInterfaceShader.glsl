#version 400
in vec2 TexCoord;
in vec3 AddColor;
flat in uint InstanceID;

layout( location = 0 ) out vec4 ColorData;
layout( location = 1 ) out unsigned int PickingTexData;

//Input textures
uniform sampler2D diffuseTex;


void main() 
{
	// Diffuse tex
	vec4 albedo_tex = texture( diffuseTex, TexCoord );

	vec4 coloradded;
	float blendFactor = 0.5f;
	if( AddColor != vec3(0.0) )
		coloradded = vec4((1.0f-blendFactor)*albedo_tex.xyz + blendFactor * AddColor, albedo_tex.a);
	else
		coloradded = albedo_tex;

	ColorData = coloradded;
	PickingTexData = InstanceID+1;
}