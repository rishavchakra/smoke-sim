#pragma once

#include "glad.h"
#include "smokesim.h"
class ParticleRenderer {
  Vector3 size;
  Vector3i resolution;
  int num_particles;
  GLuint billboard_vert_buf;
  GLuint particle_positions_buf;

  public: 
    SmokeSimulation &smoke_sim;
    ParticleRenderer(SmokeSimulation &smoke_sim, int num_particles);
    void draw(); 
    void update_buffers();
    void step();

  protected: 
    void bind_buffers(); 

};
