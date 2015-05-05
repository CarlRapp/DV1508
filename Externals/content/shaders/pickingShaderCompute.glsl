#version 440 core
layout (local_size_x = 16, local_size_y = 16) in;

// ---- Structs ---- 
//struct vector3 { float x, y, z; };
//struct DirectionalLight { vector3 Direction; vector3 Intensity; vector3 Color; };
//struct Pointlight {	vector3 Position; vector3 Intensity; vector3 Color; float Range; };

// ---- INPUTS ----
//layout (r16ui, binding = 7) uniform readonly uimage2D PickingTex;
//layout (rgba32f, binding = 2) uniform readonly image2D input_image;
layout (std430, binding = 7) buffer InstanceID { unsigned short id; };

uniform usampler2D PickingTex;

uniform int MouseX;
uniform int MouseY;

// ---- INPUT/OUTPUT ---- 
layout (rgba32f, binding = 5) uniform image2D output_image;


// ---- PER THREAD GLOBALS ---- 
ivec2 g_threadID;		// sample pixel position


// ---- MAIN ---- 
void main() 
{
	// Get threadID
	g_threadID = ivec2( gl_GlobalInvocationID.x, gl_GlobalInvocationID.y );

	if(g_threadID == ivec2(MouseX, MouseY))
	{
		id = unsigned short(texelFetch(PickingTex, g_threadID, 0).x * 65535);
	}
	id = unsigned short(5);
	vec4 FragColor = vec4(0.0f);

	uint test = texelFetch(PickingTex, g_threadID, 0).x * 65535; //imageLoad(PickingTex, g_threadID).x * 65535;

	//if(test >= 0 && test < 10)
		//FragColor = vec4(vec3(test), 1.0f);//vec4(vec3(imageLoad(PickingTex, g_threadID).x), 1.0f);


	// Fulfix tmp
	for(int x = -1; x <= 1; x++)
	{
		for(int y = -1; y <= 1; y++)
		{
			if( (g_threadID.x+x >= 0 && g_threadID.x+x < 1600) && (g_threadID.y+y >= 0 && g_threadID.y+y < 900) )
			{
				if( test != texelFetch(PickingTex, ivec2(g_threadID.x+x, g_threadID.y+y), 0).x * 65535 )
					FragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
			}
		}
	}

	// Do frag calcs here
	//vec4 FragColor = imageLoad(output_image, g_threadID); //vec4( vec3(g_depthVal), 1.0 );
	imageStore(
		output_image,
		ivec2(gl_GlobalInvocationID.xy),
		FragColor
	);
}

