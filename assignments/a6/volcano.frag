/* Fragment shader for different mesh */
// Eric Lu, COSC 077, 23W, 2/14/23

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
uniform sampler2D tex_normal;			////texture normal

/*input variables*/
in vec4 vtx_color;
in vec3 nml;
in vec3 vtx_pos;
in vec2 vtx_uv;
in vec3 vtx_tangent;

/*output variables*/
out vec4 frag_color;

/*This part is the same as your previous assignment. Here we provide a default parameter set for the hard-coded lighting environment. Feel free to change them.*/
const vec3 LIGHT_POS_BASE = vec3(0, 0, 10000);
const vec3 LightIntensity = vec3(20);
const vec3 K_A = 0.1*vec3(1., 1., 1.);
const vec3 K_D = 0.7*vec3(1., 1., 1.);
const vec3 K_S = vec3(2.);
const float P = 400.0; // n


const float PI = atan(1.0) * 4;

void main()							
{		
	vec3 light_pos = LIGHT_POS_BASE;
	vec2 uv = vtx_uv.xy;

	// Normal mapping
	vec4 texture_color = texture(tex_albedo, uv);
	vec3 texture_normals = texture(tex_normal, uv).xyz;
	vec3 n_prime = normalize(texture_normals * 2.0 - 1.0);
	vec3 vtx_bitangent = normalize(cross(nml, vtx_tangent));
	mat3 tbn = mat3(vtx_tangent, vtx_bitangent, nml);
	vec3 n_dbl_prime = normalize(tbn * n_prime);

	// Lighting
	vec3 I_a = texture_color.rgb;
	vec3 I_d = texture_color.rgb;
	vec3 I_s = texture_color.rgb;
	vec3 l_j = normalize(light_pos - vtx_pos);
	float l_dot_n = dot(l_j, n_dbl_prime);
	float diff = max(0., l_dot_n);
	vec3 v = normalize(position.xyz - vtx_pos);
	vec3 r_j = normalize(-1 * l_j + 2 * l_dot_n * n_dbl_prime);
	float spec = max(0., dot(v, r_j));
	vec3 L_0 = K_A * I_a + K_D * I_d * diff + K_S * I_s * pow(spec, P);
	frag_color = vec4(L_0.rgb, 1.f);
}	