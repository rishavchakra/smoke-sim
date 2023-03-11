#include "smokesim.h"
#include "field.h"
#include "glad.h"
#include "gtc/quaternion.hpp"
#include <iostream>

SmokeSimulation::SmokeSimulation(Vector3i size, Vector3i resolution)
    : fDensity(Field(resolution)), fTemp(Field(resolution)),
      fVel(VectorField(resolution)), size(size), resolution(resolution) {
  glGenTransformFeedbacks(2, this->feedback);

  // Bind the first set of buffers
  glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[0]);
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, buf_pos[0]);
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, buf_vel[0]);
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, buf_start_time[0]);

  // Bind the second set of buffers
  glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[1]);
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, buf_pos[1]);
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, buf_vel[1]);
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, buf_start_time[1]);
}

SmokeSimulation::~SmokeSimulation() {}

void SmokeSimulation::step(float dt) {
  this->advect_vel(dt);
  // add external velocities
  // project(?)
  this->advect_temp(dt);
  this->advect_density(dt);

  // advect rendered particles
}

void SmokeSimulation::advect_density(float dt) {
  // TODO: use an instance variable to reduce memory allocations
  Field newDensity = Field(this->resolution);
  Vector3 cellsize =
      Vector3(size.x() / resolution.x(), size.y() / resolution.y(),
              size.z() / resolution.z());

#pragma omp parallel for
  for (int x = 0; x < resolution.x(); x++) {
    for (int y = 0; y < resolution.y(); y++) {
      for (int z = 0; z < resolution.z(); z++) {
        Vector3 cellCenter =
            Vector3((x + 0.5) * cellsize.x(), (y + 0.5) * cellsize.y(),
                    (z + 0.5) * cellsize.z());
        Vector3 posVel = this->fVel.interp(cellCenter);
        Vector3 midpoint = cellCenter - (posVel * dt / 2);
        // TODO: is this supposed to be multiplied by dt?
        Vector3 oldpoint = cellCenter - this->fVel.interp(midpoint);
        newDensity.set(Vector3i(x, y, z), this->fDensity.interp(oldpoint));
      }
    }
  }

  fDensity = newDensity;
}

void SmokeSimulation::advect_temp(float dt) {
  Field newTemp = Field(this->resolution);
  Vector3 cellsize =
      Vector3(size.x() / resolution.x(), size.y() / resolution.y(),
              size.z() / resolution.z());

#pragma omp parallel for
  for (int x = 0; x < resolution.x(); x++) {
    for (int y = 0; y < resolution.y(); y++) {
      for (int z = 0; z < resolution.z(); z++) {
        Vector3 cellCenter =
            Vector3((x + 0.5) * cellsize.x(), (y + 0.5) * cellsize.y(),
                    (z + 0.5) * cellsize.z());
        Vector3 posVel = this->fVel.interp(cellCenter);
        Vector3 midpoint = cellCenter - (posVel * dt / 2);
        // TODO: is this supposed to be multiplied by dt?
        Vector3 oldpoint = cellCenter - this->fVel.interp(midpoint);
        newTemp.set(Vector3i(x, y, z), this->fTemp.interp(oldpoint));
      }
    }
  }

  fTemp = newTemp;
}

void SmokeSimulation::advect_vel(float dt) {
  VectorField newVel = VectorField(this->resolution);
  Vector3 cellsize =
      Vector3(size.x() / resolution.x(), size.y() / resolution.y(),
              size.z() / resolution.z());

  for (int x = 0; x < resolution.x(); x++) {
    for (int y = 0; y < resolution.y(); y++) {
      for (int z = 0; z < resolution.z(); z++) {
        Vector3 xFaceCenter =
            Vector3(x * cellsize.x(), (y + 0.5) * cellsize.y(),
                    (z + 0.5) * cellsize.z());
        Vector3 yFaceCenter =
            Vector3((x + 0.5) * cellsize.x(), y * cellsize.y(),
                    (z + 0.5) * cellsize.z());
        Vector3 zFaceCenter =
            Vector3((x + 0.5) * cellsize.x(), (y + 0.5) * cellsize.y(),
                    z * cellsize.z());

        Vector3 xFaceMid = xFaceCenter - fVel.interp(xFaceCenter);
        Vector3 yFaceMid = yFaceCenter - fVel.interp(yFaceCenter);
        Vector3 zFaceMid = zFaceCenter - fVel.interp(zFaceCenter);

        newVel.set(
            Vector3(x, y, z),
            Vector3(fVel.x.interp(xFaceCenter - fVel.interp(xFaceMid) * dt),
                    fVel.y.interp(xFaceCenter - fVel.interp(xFaceMid) * dt),
                    fVel.z.interp(xFaceCenter - fVel.interp(xFaceMid) * dt)));
      }
    }
  }
  fVel = newVel;
}
