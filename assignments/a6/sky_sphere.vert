/* Vertex shader for texture and normal mappings */
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

/*output variables*/
out vec3 vtx_pos;

void main()												
{
	/*camera-transformed position. do not modify.*/
	//// TODO: set your out variables
	gl_Position=pvm*vec4(pos.xyz,1.f);
	vtx_pos = pos.xyz;
}