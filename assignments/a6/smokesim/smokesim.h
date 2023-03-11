/**
 * State module of a smoke simulation
 */

#pragma once

#include "Common.h"
#include "field.h"
#include "glad.h"
#include "particle.h"
class SmokeSimulation {
  // OpenGL buffers for particle information
  GLuint feedback[2];
  GLuint buf_pos[2]; // Position buffers
  GLuint buf_vel[2]; // Velocity buffers
  GLuint buf_start_time[2];

  // The number of grid cells for physics fields
  Vector3i resolution;

  // 3d grids for each field
  Field fDensity;
  float fTemperature;
  float fVelocities;

public:
  // Constructor
  SmokeSimulation(Vector3i resolution);
  // Destructor
  ~SmokeSimulation();
  // Step forward in the physics simulation by a timestep dt
  void step(float dt);

protected:
  void init_fields(Vector3i resolution);
};