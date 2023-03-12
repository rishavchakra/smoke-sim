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
  int num_particles;

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
  std::vector<Vector3> particleColor;
  std::vector<float> pPosArray;
  std::vector<float> pVelArray;
  std::vector<float> pColArray;

  // Constructor
  SmokeSimulation(Vector3 size, Vector3i resolution, int num_particles);
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
