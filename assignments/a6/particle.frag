/*This is your fragment shader for texture and normal mapping*/

#version 330 core

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
in vec4 vtx_color;

/*uniform variables*/
uniform float iTime;					////time
uniform sampler2D tex_albedo;			////texture color
uniform sampler2D tex_normal;			////texture normal


/*output variables*/
out vec4 frag_color;

void main()							
{		
	//vec2 uv = vtx_uv.xy;
	//vec4 texture_color = texture(tex_albedo, uv);
	frag_color = vec4(vtx_color.rgb,1.f);
}	