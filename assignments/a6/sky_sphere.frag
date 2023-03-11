/* Fragment shader for texture and normal mapping*/
// Eric Lu, COSC 077, 23W, 3/10/23

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

/*uniform variables*/
uniform float iTime;					////time
uniform sampler2D tex_albedo;			////texture color

/*input variables*/
in vec3 vtx_pos;

/*output variables*/
out vec4 frag_color;

const float PI = atan(1.0) * 4;

void main()							
{		
	float x = vtx_pos.x;
	float y = vtx_pos.y;
	float z = vtx_pos.z;
	float r = sqrt(x*x + y*y + z*z);
	float theta = 0;
	if (r != 0) {
		theta = acos(z / r);
	}
	float phi = 0;
	if (x != 0 || y != 0) {
		phi = acos(x / (sqrt(x * x + y * y)));
		if (y < 0) {
			phi = -1 * phi;
		}
	}
	float u = phi / (2 * PI);
	float v = theta / PI;
	vec2 uv = vec2(u, v);
	
	vec4 texture_color = texture(tex_albedo, uv);
	frag_color = vec4(texture_color.rgb, 1.f);
}	