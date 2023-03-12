#pragma once

#include "OpenGLParticles.h"
#include "glad.h"
#include "smokesim.h"
class ParticleRender {
  OpenGLPoints points;
  int num_particles;

public:
  SmokeSimulation &smoke_sim;

  ParticleRender(SmokeSimulation &smoke_sim, int num_particles);
  void draw();
  void update_buffers();
  void step();
};

class ParticleRenderer {
  Vector3 size;
  Vector3i resolution;
  int num_particles;
  GLuint billboard_vert_buf;
  GLuint particle_positions_buf;
  GLuint particle_color_buf;

public:
  SmokeSimulation &smoke_sim;
  ParticleRenderer(SmokeSimulation &smoke_sim, int num_particles);
  void draw();
  void update_buffers();
  void step();

protected:
  void bind_buffers();
  void unbind_buffers();
};
