/* Vertex shader for different mesh */
// Eric Lu, COSC 077, 23W, 2/14/23

#version 330 core

#define PI 3.14159265

/*default camera matrices. do not modify.*/
layout (std140) uniform camera
{
	mat4 projection;	/*camera's projection matrix*/
	mat4 view;			/*camera's view matrix*/
	mat4 pvm;			/*camera's projection*view*model matrix*/
	mat4 ortho;			/*camera's ortho projection matrix*/
	vec4 position;		/*camera's position in world space*/
};

/*input variables*/


layout (location=0) in vec4 pos;		/*vertex position*/
layout (location=1) in vec4 v_color;	/*vertex color*/
layout (location=2) in vec4 normal;		/*vertex normal*/	////ATTENTION: this attribute is important for TBN matrix calculation!
layout (location=3) in vec4 uv;			/*vertex uv*/		////ATTENTION: this is the texture coordinates!
layout (location=4) in vec4 tangent;	/*vertex tangent*/	////ATTENTION: this attribute is important for TBN matrix calculation!

/*output variables*/
//// TODO: declare your output variables
out vec4 vtx_color;
out vec3 nml;
out vec3 vtx_pos;
out vec2 vtx_uv;
out vec3 vtx_tangent;

out vec3 frag_pos;
out vec3 tangent_view_pos;
out vec3 tangent_frag_pos;

void main()												
{
	/*camera-transformed position. do not modify.*/
	//// TODO: set your out variables
	gl_Position=pvm*vec4(pos.x - 3000, pos.y - 2000, pos.z + 2000, 1.f);
	vtx_color=vec4(v_color.rgb,1.f);
	nml=normal.xyz;
	vtx_pos = pos.xyz;
	vtx_uv = uv.xy;
	vtx_tangent = tangent.xyz;

	frag_pos = vec3(pvm * pos);
	vec3 T = normalize(mat3(pvm) * tangent.xyz);
	vec3 B = normalize(mat3(pvm) * normalize(cross(normal.xyz, tangent.xyz)));
	vec3 N = normalize(mat3(pvm) * normal.xyz);
	mat3 TBN = transpose(mat3(T, B, N));
	tangent_view_pos = TBN * position.xyz;
	tangent_frag_pos = TBN * frag_pos;
}	
