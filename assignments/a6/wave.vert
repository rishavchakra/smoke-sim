#version 330 core

// Add parts 1a-c first, then implement part 2a
// Eric Lu, COSC 077, 23W, 3/10/23

layout (std140) uniform camera
{
    mat4 projection;
    mat4 view;
    mat4 pvm;
    mat4 ortho;
    vec4 position;
};

/*uniform variables*/
uniform float iTime; ////time

/*input variables*/
layout (location=0) in vec4 pos;
layout (location=2) in vec4 normal;
layout (location=3) in vec4 uv;
layout (location=4) in vec4 tangent;

/*output variables*/
out vec3 vtx_pos; ////vertex position in the world space

///////////// Part 1a /////////////////////
/* Create a function that takes in an xy coordinate and returns a 'random' 2d vector. (There is no right answer)
   Feel free to find a hash function online. Use the commented function to check your result */
vec2 hash2(vec2 v)
{
    vec2 rand = vec2(0,0);

    // Your implementation starts here
    // From Book of Shaders
    float n = fract(sin(dot(v.xy, vec2(12.9898,78.233))) * 43758.5453123);
    rand = vec2(cos(n * 2.0 * 3.14159265), sin(n * 2.0 * 3.14159265)); // Return as angle
    // Your implementation ends here

    // rand  = 50.0 * 1.05 * fract(v * 0.3183099 + vec2(0.71, 0.113)); // Sample provided hash
    // rand = -1.0 + 2 * 1.05 * fract(rand.x * rand.y * (rand.x + rand.y) * rand); // Sample provided hash

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

void main()
{
    vtx_pos = (vec4(pos.x - 10000, pos.y - 10000, water(pos.xy), 1)).xyz;
    gl_Position = pvm * vec4(vtx_pos,1);
}
