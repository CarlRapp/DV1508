#version 430 core

layout (local_size_x = 16, local_size_y = 16) in;

//input textures (G-buffer textures)
layout (rgba32f, binding = 0) uniform readonly image2D NormalTex;
layout (rgba8, binding = 1) uniform readonly image2D ColorTex;

// random vector
layout (rgba8, binding = 2) uniform readonly image2D random_vector;

// output 
layout (rgba32f, binding = 5) uniform image2D output_image;

uniform sampler2D DepthTex;

const int nrOfLights = 1;
struct LightInfo {
	vec4 Position; // Light position in world coords.
	vec3 Intensity; // Diffuse intensity
	vec3 Color;
	float Range;
};
//uniform 
LightInfo Lights[nrOfLights];

struct MaterialInfo {
	float Ks;
	float Shininess;
};
//uniform 
MaterialInfo Material;

uniform mat4 ViewMatrix;
uniform mat4 invProjection;

ivec2 texelCoord;
vec3 viewPos;
vec3 normal_tex;
vec3 albedo_tex;
float depthVal_tex;

vec2 getRandom()
{
	return normalize( imageLoad(random_vector, ivec2(gl_LocalInvocationID.xy)).xy * 2.0f - 1.0f );
}
/*
float doAmbientOcclusion( vec2 offset )
{
	float g_scale = 8;
	float g_intensity = 12;
	float g_bias = 0.00;

	texelCoord;

	vec3 diff = getPosition(texelCoord + offset) - p;
	vec3 v = normalize(diff);
	float d = length(diff)*g_scale;
	return max(0.0,dot(cnorm,v)-g_bias)*(1.0/(1.0+d))*g_intensity;
}

float ComputeSSAO()
{
	float g_sample_rad = 1.2;
	vec2 vec[4] = { vec2(1,0), vec2(-1,0), vec2(0,1), vec2(0,-1) };

	vec2 rand = getRandom();

	float ao = 0.0f;
	float rad = g_sample_rad/viewPos.z;

	//SSAO Calculation
	for (int j = 0; j < 4; ++j)
	{
		vec2 coord1 = reflect(vec[j],rand)*rad;
		vec2 coord2 = vec2(coord1.x*0.707 - coord1.y*0.707, coord1.x*0.707 + coord1.y*0.707);
  
		ao += doAmbientOcclusion(uv,coord1*0.25, p, n);
		ao += doAmbientOcclusion(uv,coord2*0.5, p, n);
		ao += doAmbientOcclusion(uv,coord1*0.75, p, n);
		ao += doAmbientOcclusion(uv,coord2, p, n);
	} 

	ao/= 16;

	return 1-ao;
}
*/
void phongModel(int index, out vec3 ambient, out vec3 diffuse, out vec3 spec) 
{
        //tmp material
        //Material.Ks = 0.7;
        //Material.Shininess = 30.0;

        ambient = vec3(0.0);
        diffuse = vec3(0.0);
        spec    = vec3(0.0);

        vec3 lightVec = (ViewMatrix * Lights[index].Position).xyz - viewPos;

        float d = length(lightVec);

        if(d > Lights[index].Range)
            return;
        lightVec /= d; //normalizing
        

		ambient = Lights[index].Color * Lights[index].Intensity.x;

		vec3 E = normalize(-viewPos);
		vec3 N = normalize(normal_tex);

		float diffuseFactor = dot( lightVec, N );

		if(diffuseFactor > 0)
		{
			// diffuse
			diffuse = diffuseFactor * Lights[index].Color * Lights[index].Intensity.y;

			// specular
			vec3 v = normalize(2 * Material.Ks * N - lightVec);//reflect( lightVec, normal_tex );
			float specFactor = max( pow( max( dot(v, E), 0.0f ), Material.Shininess ), 0.0f);
			spec = specFactor * Lights[index].Color * Lights[index].Intensity.z * Material.Ks;        
		}

		float att = 1 - pow((d/Lights[index].Range), 1.0f);

		ambient *= att;
		diffuse *= att;
		spec    *= att;

		return;
}

vec3 reconstructPosition(float p_depth, vec2 p_ndc)
{ 
    vec4 sPos = vec4(p_ndc, p_depth*2.0f - 1.0f, 1.0);
    sPos = invProjection * sPos;

    return (sPos.xyz / sPos.w);
}

void main() 
{
	Lights[0].Position = vec4(0.0, 1.0, 3.0, 1.0);
	Lights[0].Intensity = vec3(0.2, 0.9, 0.9);
	Lights[0].Color = vec3(0.9);
	Lights[0].Range = 100.0f;

	texelCoord = ivec2( gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);

	// Retrieve position, normal and color information from the g-buffer textures
	vec4 inputMap0 = texelFetch(DepthTex, texelCoord, 0);
	vec4 inputMap1 = imageLoad(NormalTex, texelCoord);
	vec4 inputMap2 = imageLoad(ColorTex, texelCoord);

	// Normal haxxx
	normal_tex.xy = inputMap1.xy;
	normal_tex.z = sqrt( 1 - (normal_tex.x*normal_tex.x) - (normal_tex.y*normal_tex.y) );
	//--------------
	Material.Ks = inputMap1.z;
    Material.Shininess = inputMap1.w * 254.0f + 1.0f;
	//--------------
	albedo_tex.xyz = inputMap2.xyz;
	//--------------
	float glow = inputMap2.w;
	//--------------
	depthVal_tex = inputMap0.x;

	vec2 ndc = vec2(texelCoord) / vec2( gl_WorkGroupSize.xy*gl_NumWorkGroups.xy ) * 2.0f - 1.0f;
	viewPos = reconstructPosition(depthVal_tex, ndc);

	vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 spec    = vec3(0.0);

	//f�r varje ljus-----------
	for(int i = 0; i < nrOfLights; i++)
	{
		vec3 a,d,s;

		phongModel(i, a, d, s);
		ambient += a;
		diffuse += d;
		spec    += s;
	}
	//-------------------------

	//DIR LIGHT ---------------
	vec3 lightDir = (ViewMatrix * vec4(-3, 4, -2, 0)).xyz;
	float lightIntensity = max(dot(normalize(lightDir), normalize(normal_tex)), 0.0 );;
	if (lightIntensity > 0.0f)
		diffuse += (vec3(0.6, 0.6, 0.6) * lightIntensity);
	//-------------------------

	vec4 FragColor = vec4(ambient + diffuse, 1.0) * vec4(albedo_tex, 1.0) + vec4(spec, 0.0f);
	//FragColor = vec4(diffuse, 1.0);
	//vec4 FragColor = vec4( normal_tex, 1.0);
	//vec4 FragColor = vec4( albedo_tex   +normal_tex-normal_tex, 1.0 );
	//vec4 FragColor = vec4( vec3(viewPos  +normal_tex-normal_tex), 1.0 );
	//vec4 FragColor = vec4( vec3(depthVal_tex), 1.0 );
	imageStore(
		output_image,
		ivec2(gl_GlobalInvocationID.xy),
		FragColor
	);
}