/*This is your first vertex shader!*/

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
layout (location=1) in vec4 color;		/*vertex color*/
layout (location=2) in vec4 normal;		/*vertex normal*/	
layout (location=3) in vec4 uv;			/*vertex uv*/		
layout (location=4) in vec4 tangent;	/*vertex tangent*/

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

uniform mat4 model;						////model matrix
uniform float iTime;					////time

void main()												
{
	float speed = iTime * 50;
	gl_Position=pvm*model*vec4(pos.x, pos.y - 10000 + speed, pos.z, 1.f);
	vtx_color=vec4(vtx_color.rgb,1.f);
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