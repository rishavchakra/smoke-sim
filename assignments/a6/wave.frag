#version 330 core
// Eric Lu, COSC 077, 23W, 2/20/23
layout (std140) uniform camera
{
	mat4 projection;
	mat4 view;
	mat4 pvm;
	mat4 ortho;
	vec4 position;
};
/*uniform variables*/
uniform float iTime;					////time
uniform sampler2D tex_albedo;			////texture color
/*input variables*/
in vec3 vtx_pos;

/*input variables*/
out vec4 frag_color;

const float PI = atan(1.0) * 4;

///////////// Part 1a /////////////////////
/* Create a function that takes in an xy coordinate and returns a 'random' 2d vector. (There is no right answer)
   Feel free to find a hash function online. Use the commented function to check your result */
vec2 hash2(vec2 v)
{
	vec2 rand = vec2(0,0);
	
	// Your implementation starts here
	// From Book of Shaders
	// float n = fract(sin(dot(v.xy, vec2(12.9898,78.233))) * 43758.5453123);
	// rand = vec2(cos(n * 2.0 * 3.14159265), sin(n * 2.0 * 3.14159265)); // Return as angle
	// Your implementation ends here
	
	rand  = 50.0 * 1.05 * fract(v * 0.3183099 + vec2(0.71, 0.113)); // Sample provided hash
    rand = -1.0 + 2 * 1.05 * fract(rand.x * rand.y * (rand.x + rand.y) * rand); // Sample provided hash
	return rand;
}
///////////// Part 1b /////////////////////
/*  Using i, f, and m, compute the perlin noise at point v */
float perlin_noise(vec2 v)
{
	float noise = 0;
	// Your implementation starts here
	vec2 i = floor(v);
	vec2 f = fract(v);
	vec2 a = hash2(i);
	vec2 b = hash2(i + vec2(1.0, 0.0));
	vec2 c = hash2(i + vec2(0.0, 1.0));
	vec2 d = hash2(i + vec2(1.0, 1.0));
	vec2 q = f;
	vec2 r = f - vec2(1.0, 0.0);
	vec2 s = f - vec2(0.0, 1.0);
	vec2 t = f - vec2(1.0, 1.0);
	
	vec2 step = f*f*(3.0-2.0*f);
	float m1 = mix(dot(a, q), dot(b, r), step.x);
	float m2 = mix(dot(c, s), dot(d, t), step.x);
	noise = mix(m1, m2, step.y);
	// Your implementation ends here
	return noise;
}
///////////// Part 1c /////////////////////
/*  Given a point v and an int num, compute the perlin noise octave for point v with octave num
	num will be greater than 0 */
float noiseOctave(vec2 v, int num)
{
	float sum = 0;
	// Your implementation starts here
	for (int i = 0; i < num; i++) {
		float w_i = pow(2.0, -1.0 * i);
		float s_i = pow(2.0, 1.0 * i);
		sum += w_i * perlin_noise(s_i * v);
	}
	// Your implementation ends here
	return sum;
}
///////////// Part 2a /////////////////////
/* create a function that takes in a 2D point and returns a height using perlin noise
    There is no right answer. Think about what functions will create what shapes.
    If you want steep mountains with flat tops, use a function like sqrt(noiseOctave(v,num)).
    If you want jagged mountains, use a function like e^(noiseOctave(v,num))
    You can also add functions on top of each other and change the frequency of the noise
    by multiplying v by some value other than 1*/
float water(vec2 p) {
    float h = 0;
    vec2 sh1 = 0.001 * vec2(iTime * 320.0, iTime * 240.0);
    vec2 sh2 = 0.001 * vec2(iTime * 380.0, iTime * -260.0);

    float wave = 0.0;
    wave += sin(p.x * 0.022 + sh2.x) * 4.4;
    wave += sin(p.x * 0.0170 + p.y * 0.010 + sh2.x * 1.120) * 4.0;
    wave -= sin(p.x * 0.001 + p.y * 0.005 + sh2.x * 0.120) * 4.0;

    wave += sin(p.x * 0.022 + p.y * 0.012 + sh2.x * 3.44) * 5.0;
    wave += sin(p.x * 0.031 + p.y * 0.012 + sh2.x * 4.28) * 2.5 ;
    wave *= 1.0;
    wave -= noiseOctave(p * 0.005 - sh2 * 0.5, 6) * 1.0 * 25.;
    
    h += wave;
    return h;
}

///////////// Part 2b /////////////////////
/* compute the normal vector at v by find the points d to the left/right and d forward/backward
    and using a cross product. Be sure to normalize the result */
vec3 compute_normal(vec2 v, float d)
{	
	vec3 normal_vector = vec3(0,0,0);
	// Your implementation starts here
	vec3 v1 = vec3(v.x + d, v.y, water(vec2(v.x + d, v.y)));
    vec3 v2 = vec3(v.x - d, v.y, water(vec2(v.x - d, v.y)));
    vec3 v3 = vec3(v.x, v.y + d, water(vec2(v.x, v.y + d)));
    vec3 v4 = vec3(v.x, v.y - d, water(vec2(v.x, v.y - d)));
	normal_vector = normalize(cross(v3 - v1, v2 - v1));
	// Your implementation ends here
	return normal_vector;
}

vec4 sphere_map(vec3 n) {
	float x = n.x;
	float y = n.y;
	float z = n.z;
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
	return vec4(texture_color.rgb, 1.f);
}

///////////// Part 2c /////////////////////
/* complete the get_color function by setting emissiveColor using some function of height/normal vector/noise */
/* put your Phong/Lambertian lighting model here to synthesize the lighting effect onto the terrain*/
vec3 get_color(vec3 v)
{
	// Your implementation starts here
	
	/*This part is the same as your previous assignment. Here we provide a default parameter set for the hard-coded lighting environment. Feel free to change them.*/
	const vec3 LightPosition = vec3(0, 0, 10000);
	const vec3 LightIntensity = vec3(100000000);
	const vec3 ka = 0.1*vec3(1., 1., 1.);
	const vec3 kd = 0.7*vec3(1., 1., 1.);
	const vec3 ks = vec3(100.);
	const float n = 40.0;
	vec3 vtx_normal = compute_normal(v.xy, 0.01);
	// vec3 vtx_normal = compute_water_normals(v.xy);
	vec3 normal = normalize(vtx_normal.xyz);

	vec3 lightDir = LightPosition - vtx_pos;
	float _lightDist = length(lightDir);
	vec3 _lightDir = normalize(lightDir);
	vec3 _localLight = LightIntensity / (_lightDist * _lightDist);
	vec3 ambColor = ka;
	vec3 lightingColor = kd * _localLight * max(0., dot(_lightDir, normal));

	vec3 emissiveColor = mix(vec3(0, 0.25, 0.5), vec3(0, 0.25, 0.9), 1 - max(0., dot(_lightDir, normal)));
	vec3 rflect = reflect(_lightDir, normal);
	vec4 reflectColor = sphere_map(rflect);
	vec3 rfract = refract(_lightDir, normal, 1.33);
	vec4 refractColor = sphere_map(rfract);
	float R_0 = 0.02037;
	float fresnelCoeff = R_0 + (1 - R_0) * pow(1 - max(0., dot(_lightDir, normal)), 5.);

	emissiveColor = emissiveColor + reflectColor.rgb * fresnelCoeff + refractColor.rgb * (1 - fresnelCoeff);

	// Your implementation ends here
    return emissiveColor*lightingColor;
	// return emissiveColor;
}
void main()
{
	frag_color = vec4(get_color(vtx_pos),1.f);
}