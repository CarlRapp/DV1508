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
uniform int Clicked;

// ---- INPUT/OUTPUT ---- 
layout (rgba32f, binding = 5) uniform image2D output_image;


// ---- PER THREAD GLOBALS ---- 
ivec2 g_threadID;		// sample pixel position


// ---- MAIN ---- 
void main() 
{
	// Get threadID
	g_threadID = ivec2( gl_GlobalInvocationID.x, gl_GlobalInvocationID.y );

	if(Clicked == 1)
	{
		if(g_threadID == ivec2(MouseX, MouseY))
		{
			id = texelFetch(PickingTex, g_threadID, 0).x-1; // * 4294967296;
		}
	}
	//id = unsigned short(5);
	vec4 FragColor = imageLoad(output_image, g_threadID);

	//float test = round(texelFetch(PickingTex, g_threadID, 0).x * 4294967296.f); //imageLoad(PickingTex, g_threadID).x * 65535;
	//if(id == 1)
	//	FragColor = vec4(0.3f, 0.0f, 1.0f, 1.0f);

	//if(test >= 0 && test < 10)
		//FragColor = vec4(vec3(test), 1.0f);//vec4(vec3(imageLoad(PickingTex, g_threadID).x), 1.0f);

	//if(int(test) >= 1)
	//	FragColor = vec4(0.3f, 0.0f, 1.0f, 1.0f);
	//else if(test == 0.0f)
	//	FragColor = vec4(0.0, 1.0, 0.0, 1.0);

	// Fulfix tmp hårdkodad
	if(PickedID >= 0)
	{
		if(PickedID == texelFetch(PickingTex, g_threadID, 0).x-1)
			FragColor = FragColor+vec4(0.2, 0.2, 0.2, 0.0);
		//for(int x = -1; x <= 1; x++)
		//{
		//	for(int y = -1; y <= 1; y++)
		//	{
		//		if( (g_threadID.x+x >= 0 && g_threadID.x+x < 1600) && (g_threadID.y+y >= 0 && g_threadID.y+y < 900) )
		//		{
		//			if( id != texelFetch(PickingTex, ivec2(g_threadID.x+x, g_threadID.y+y), 0).x )
		//				FragColor = vec4(0.5f, 0.0f, 1.0f, 1.0f);
		//		}
		//	}
		//}
	}

	// Do frag calcs here
	//FragColor = imageLoad(output_image, g_threadID); //vec4( vec3(g_depthVal), 1.0 );
	imageStore(
		output_image,
		ivec2(gl_GlobalInvocationID.xy),
		FragColor
	);

	barrier();

}

