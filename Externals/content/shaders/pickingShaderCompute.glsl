#version 440 core
layout (local_size_x = 16, local_size_y = 16) in;

// ---- Structs ---- 
//struct vector3 { float x, y, z; };
//struct DirectionalLight { vector3 Direction; vector3 Intensity; vector3 Color; };
//struct Pointlight {	vector3 Position; vector3 Intensity; vector3 Color; float Range; };

// ---- INPUTS ----
//layout (r16ui, binding = 7) uniform readonly uimage2D PickingTex;
//layout (rgba32f, binding = 2) uniform readonly image2D input_image;
layout (std430, binding = 7) buffer InstanceID { uint id; };

uniform usampler2D PickingTex;

uniform int MouseX;
uniform int MouseY;
uniform int PickedID;
uniform int HoverID;
uniform int Clicked;

// ---- INPUT/OUTPUT ---- 
layout (rgba32f, binding = 5) uniform image2D output_image;


// ---- PER THREAD GLOBALS ---- 
ivec2 g_threadID;		// sample pixel position


// ---- MAIN ---- 
void main() 
{
	g_threadID = ivec2( gl_GlobalInvocationID.x, gl_GlobalInvocationID.y );

	if(g_threadID == ivec2(MouseX, MouseY))
	{
		id = texelFetch(PickingTex, g_threadID, 0).x-1;
	}

	vec4 FragColor = imageLoad(output_image, g_threadID);

	if (PickedID >= 0 || HoverID >= 0 )
	{
		uint pixelID = texelFetch(PickingTex, g_threadID, 0).x - 1;

		if (PickedID == pixelID && PickedID >= 0) //Picked
		{
			FragColor = FragColor + vec4(0.8, 0.1, 0.1, 0.0);
		}
		else if (HoverID == pixelID && HoverID >= 0) //Hovered
		{
			FragColor = FragColor + vec4(0.2, 0.2, 0.2, 0.0);
		}
	}

	imageStore(
		output_image,
		ivec2(gl_GlobalInvocationID.xy),
		FragColor
	);

	barrier();

}

