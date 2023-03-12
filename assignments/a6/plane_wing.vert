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

uniform mat4 model;						////model matrix
uniform float iTime;					////time

/*output variables*/
out vec2 vtx_uv;

void main()												
{
	float theta = iTime * 2;
	gl_Position=pvm*model*vec4(pos.x + 100*cos(theta), pos.y + 100*sin(theta), pos.z, 1.f);
    vtx_uv = uv.xy;
}