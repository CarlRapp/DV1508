#version 400
layout( location = 0 ) in vec3 VertexPosition;
layout( location = 1 ) in vec3 VertexNormal;
layout( location = 2 ) in vec3 VertexTangent;
layout( location = 3 ) in vec3 VertexBiTangent;
layout( location = 4 ) in vec2 VertexTexCoord;
layout( location = 5 ) in vec4 VertexJointIndex;
layout( location = 6 ) in vec4 VertexJointWeight;

uniform mat4 M;
uniform mat4 VP;
uniform mat3 NormalMatrix;

struct JointMatrix
{
	float x0, y0, z0, w0;
	float x1, y1, z1, w1;
	float x2, y2, z2, w2;
	float x3, y3, z3, parent;
};

//flat out int instanceID;
layout (std430, binding = 1) buffer Joints   
{
	JointMatrix joints[];
};

layout (std430, binding = 2) buffer Animation   
{
	JointMatrix anim[];
};

out vec3 Normal;
out vec3 Tan;
out vec3 BiTan;
out vec2 TexCoord;
out vec3 ViewPos;

mat4 JointToMatrix(JointMatrix joint)
{
	return mat4(
				joint.x0, joint.y0, joint.z0, joint.w0,
				joint.x1, joint.y1, joint.z1, joint.w1,
				joint.x2, joint.y2, joint.z2, joint.w2,
				joint.x3, joint.y3, joint.z3, 1
				);
}

mat4 GetJointMatrix(int Index)
{
	mat4 joint = mat4(1);
	for (int i = Index; i < anim.length(); i++)
	{
		joint = JointToMatrix(anim[i]) * joint;
		i += int(anim[i].parent);
	}
	return joint;	
}

void main()
{
	TexCoord = VertexTexCoord;

	vec4 weights = normalize(VertexJointWeight);

	mat4 skin = mat4(0);
	skin += (GetJointMatrix(anim.length()-int(VertexJointIndex.x)-1) * (JointToMatrix(joints[int(VertexJointIndex.x)]))) * weights.x;
	skin += (GetJointMatrix(anim.length()-int(VertexJointIndex.y)-1) * (JointToMatrix(joints[int(VertexJointIndex.y)]))) * weights.y;
	skin += (GetJointMatrix(anim.length()-int(VertexJointIndex.z)-1) * (JointToMatrix(joints[int(VertexJointIndex.z)]))) * weights.z;
	skin += (GetJointMatrix(anim.length()-int(VertexJointIndex.w)-1) * (JointToMatrix(joints[int(VertexJointIndex.w)]))) * weights.w;
	
	gl_Position = VP * M * skin * vec4(VertexPosition, 1.0);

	Normal = normalize( NormalMatrix * (skin * vec4(VertexNormal, 0.0)).xyz );
	Tan = normalize( NormalMatrix * (skin * vec4(VertexTangent, 0.0)).xyz );
	BiTan = normalize( NormalMatrix * (skin * vec4(VertexBiTangent, 0.0)).xyz );
	//instanceID = gl_InstanceID;
}