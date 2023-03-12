
#pragma once 

#include "Common.h"

struct Particle{
	glm::vec3 pos, speed;
	unsigned char r,g,b,a; // Color
	float size, angle, weight;
	float life; // Remaining life of the particle. if < 0 : dead and unused.
};

int MaxParticles;
Particle ParticlesContainer[MaxParticles];
int LastUsedParticle;
int FindUnusedParticle(Particle ParticlesContainer[]); 