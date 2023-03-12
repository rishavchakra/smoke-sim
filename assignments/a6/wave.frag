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
/*input variables*/
in vec3 vtx_pos;
/*input variables*/
out vec4 frag_color;

// struct ray {
//     vec3 o;
//     vec3 d;
// };

// struct light {
//    vec3 position;		////point light position
//    vec3 color;			////point light color
// };

// struct hit {
//     float t;			////parameter in the ray function
//     vec3 p;				////intersection point
//     vec3 normal;		////normal on the intersection point
//     vec3 color;			////color of the intersecting object
// };

// const float minT = 0.001;
// const float maxT = 1e8;
// const int numberOfSampling = 50;
// ////if no hit is detected, return dummyHit
// const hit dummyHit = hit(-1.0, vec3(0), vec3(0), vec3(0));
// ////calculate the ray for a given uv coordinate
// ray getRay(camera c, vec2 uv)
// {
//     return ray(c.origin, c.LowerLeftCorner + uv.x * c.horizontal + uv.y * c.vertical - c.origin);
// }

// hit hitPlane(ray r, vec3 p, vec3 n) {
// 	hit h;
// 	float t = -1 * (dot(r.o - p), n) / (dot(r.d, n));
// 	if (t > 0) {
// 		h.t = t;
// 	}
// 	else {
// 		h.t = dummyHit.t;
// 	}

// 	h.p = r.o + h.t * r.d;
// 	h.normal = n;
// 	return h;
// }

// vec3 hitColor(ray r, vec3 p, vec3 n, light l) {
// 	float K_A = 0.25;
// 	float K_D = 1;
	
// 	vec3 L_sum = vec3(0);
// 	hit h = hitPlane(r, p, n);
// 	if (h.t > 0) {
// 		vec3 I_a = h.color;
// 		vec3 I_d = h.color;

// 		ray shadowRay = ray(h.p, (l.position - h.p));
// 		hit shadowHit = hitPlane(shadowRay, p, n);
// 		if (shadowHit.t < 0) {
// 			vec3 l_j = normalize(l.position - h.p);
// 			float l_dot_n = dot(l_j, n);
// 			float diff = max(0, l_dot_n);
// 			L_sum = L_sum + K_D * I_d * diff;
// 		}
// 		L_sum = L_sum + K_A * I_A
// 	}

// 	return L_sum;
// }

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
float height(vec2 v){
    float h = 0;
	// Your implementation starts here
	h = 0.75 * noiseOctave(v, 3);
	if (h < 0) {
		h = -0.5 * h;
	}
	// Your implementation ends here
	return h;
}

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

///////////// Part 2c /////////////////////
/* complete the get_color function by setting emissiveColor using some function of height/normal vector/noise */
/* put your Phong/Lambertian lighting model here to synthesize the lighting effect onto the terrain*/
vec3 get_color(vec2 v)
{
	float h = water(v);
	vec3 vtx_normal = compute_normal(v, 0.01);
	vec3 emissiveColor = mix(vec3(0, 0.25, 0.6), vec3(0, 0.25, 0.9), h / 100);
	vec3 lightingColor= vec3(1.,1.,1.);
	// Your implementation starts here
	
	/*This part is the same as your previous assignment. Here we provide a default parameter set for the hard-coded lighting environment. Feel free to change them.*/
	const vec3 LightPosition = vec3(0, 0, 10000);
	const vec3 LightIntensity = vec3(200000000);
	const vec3 ka = 0.1*vec3(1., 1., 1.);
	const vec3 kd = 0.7*vec3(1., 1., 1.);
	const vec3 ks = vec3(2.);
	const float n = 400.0;
	vec3 normal = normalize(vtx_normal.xyz);
	vec3 lightDir = LightPosition - vtx_pos;
	float _lightDist = length(lightDir);
	vec3 _lightDir = normalize(lightDir);
	vec3 _localLight = LightIntensity / (_lightDist * _lightDist);
	vec3 ambColor = ka;
	lightingColor = kd * _localLight * max(0., dot(_lightDir, normal));
	// Your implementation ends here
    return emissiveColor*lightingColor;
	// return emissiveColor;
}
void main()
{
	frag_color = vec4(get_color(vtx_pos.xy),1.f);
	// frag_color = vec4(0.0, 0.0, 1.0, 1.f);
}