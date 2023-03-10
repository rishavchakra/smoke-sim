/* Fragment shader for texture and normal mapping*/
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

//// TODO: declare your input variables

/*output variables*/
out vec4 frag_color;

/*This part is the same as your previous assignment. Here we provide a default parameter set for the hard-coded lighting environment. Feel free to change them.*/
const vec3 LIGHT_POS = vec3(3, 1, 3);
const vec3 LightIntensity = vec3(20);
const vec3 K_A = 0.1*vec3(1., 1., 1.);
const vec3 K_D = 0.7*vec3(1., 1., 1.);
const vec3 K_S = vec3(2.);
const float P = 400.0; // n

const float PI = atan(1.0) * 4;

void main()							
{		
	bool use_step_one = false;		    // set this flag to be false when you move from Step 1 to Step 2
	bool use_normal_mapping = true;	// set this flag to be true when you move from Step 2 to Step 3
	
	vec2 uv = vtx_uv.xy;
	if (uv.x == 0 && uv.y == 0) {
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
		uv = vec2(u, v);
	}

	if (!use_normal_mapping){
		//// TODO (Step 1 and 2): texture with shading
		////Here are some useful hints:
		////Step 1.0: load the texture color from tex_albedo and then set it to frag_color
		////Step 2.0: use the loaded texture color as the local material color and multiply the local color with the Lambertian shading model you implemented previously to render a textured and shaded sphere.
		////The way to read a texture is to call texture(texture_name,uv). It will return a vec4.

		vec4 texture_color = texture(tex_albedo, uv);
		
		vec3 I_a = texture_color.rgb;
		vec3 I_d = texture_color.rgb;
		vec3 I_s = texture_color.rgb;
		
		vec3 l_j = normalize(LIGHT_POS - vtx_pos);
		float l_dot_n = dot(l_j, nml);
		float diff = max(0., l_dot_n);
	
		vec3 v = normalize(position.xyz - vtx_pos);
		vec3 r_j = normalize(-1 * l_j + 2 * l_dot_n * nml);
		float spec = max(0., dot(v, r_j));
	
		vec3 L_0 = K_A * I_a + K_D * I_d * diff + K_S * I_s * pow(spec, P);
		frag_color = vec4(L_0.rgb, 1.f);

		if (use_step_one) {
			frag_color = vec4(texture_color.rgb, 1.f);
		}
	}
	else {
		//// TODO (Step 3): texture with normal mapping
		////Here are some useful hints:
		////Step 3.0: load the texture color from tex_albedo
		////Step 3.1: load the texture normal from tex_normal, and remap each component from [0, 1] to [-1, 1] (notice that currently the loaded normal is in the local tangent space)
		////Step 3.2: calculate the TBN matrix using the vertex normal and tangent
		////Step 3.3: transform the texture normal from the local tangent space to the global world space
		////Step 3.4 and following: use the transformed normal and the loaded texture color to conduct the further lighting calculation
		////The way to declare a 3x3 matrix is mat3 mat=mat3(v0,v1,v2);
		////The way to read a texture is to call texture(texture_name,uv). It will return a vec4.
		////The way to calculate cross product is to call cross(u1,u2);
		
		vec4 texture_color = texture(tex_albedo, uv);
		vec3 texture_normals = texture(tex_normal, uv).xyz;
		vec3 n_prime = normalize(texture_normals * 2.0 - 1.0);

		vec3 vtx_bitangent = normalize(cross(nml, vtx_tangent));
		mat3 tbn = mat3(vtx_tangent, vtx_bitangent, nml);

		vec3 n_dbl_prime = normalize(tbn * n_prime);
		
		////////////

		vec3 I_a = texture_color.rgb;
		vec3 I_d = texture_color.rgb;
		vec3 I_s = texture_color.rgb;
		
		vec3 l_j = normalize(LIGHT_POS - vtx_pos);
		float l_dot_n = dot(l_j, n_dbl_prime);
		float diff = max(0., l_dot_n);
	
		vec3 v = normalize(position.xyz - vtx_pos);
		vec3 r_j = normalize(-1 * l_j + 2 * l_dot_n * n_dbl_prime);
		float spec = max(0., dot(v, r_j));
	
		vec3 L_0 = K_A * I_a + K_D * I_d * diff + K_S * I_s * pow(spec, P);
		frag_color = vec4(L_0.rgb, 1.f);
	}
}	