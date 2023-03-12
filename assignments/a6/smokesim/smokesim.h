/**
 * State module of a smoke simulation
 */

#pragma once

#include "Common.h"
#include "field.h"
#include "glad.h"
#include "particle.h"
struct SmokeSimulation {
  // OpenGL buffers for particle information
  GLuint feedback[2];
  GLuint buf_pos[2]; // Position buffers
  GLuint buf_vel[2]; // Velocity buffers
  GLuint buf_start_time[2];

  // The number of grid cells for physics fields
  Vector3 size;
  Vector3i resolution;

  // 3d grids for each field
  Field fDensity;
  Field fTemp;
  VectorField fVel;
  // Swap buffers
  Field newFDensity;
  Field newFTemp;
  VectorField newFVel;

  std::vector<Vector3> particlePos;
  std::vector<Vector3> particleVel;

  // Constructor
  SmokeSimulation(Vector3 size, Vector3i resolution, int numParticles);
  // Destructor
  ~SmokeSimulation();
  // Step forward in the physics simulation by a timestep dt
public:
  void step(float dt);

protected:
  void advect_density(float dt);
  void advect_temp(float dt);
  void advect_vel(float dt);
  void advect_particles(float dt);
};
