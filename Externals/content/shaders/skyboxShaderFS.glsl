#version 400

in vec3 TexCoord;
layout( location = 0 ) out vec4 ColorData;
layout( location = 1 ) out unsigned int PickingTexData;

uniform samplerCube cubemap;

void main()
{
	ColorData = texture(cubemap, TexCoord);
	PickingTexData = 0;
}