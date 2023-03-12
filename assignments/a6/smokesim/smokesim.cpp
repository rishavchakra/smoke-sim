#include "smokesim.h"
#include "field.h"
#include "glad.h"
#include "gtc/quaternion.hpp"
#include "particle.h"
#include <cstdlib>
#include <iostream>

SmokeSimulation::SmokeSimulation(Vector3 size, Vector3i resolution,
                                 int numParticles)
    : fDensity(Field(size, resolution)), fTemp(Field(size, resolution)),
      fVel(VectorField(size, resolution)), newFDensity(Field(size, resolution)),
      newFTemp(Field(size, resolution)), newFVel(VectorField(size, resolution)),
      size(size), resolution(resolution), num_particles(numParticles),
      particlePos(std::vector<Vector3>(numParticles, Vector3())),
      particleVel(std::vector<Vector3>(numParticles, Vector3())),
      particleColor(std::vector<Vector3>(numParticles, Vector3())),
      pPosArray(std::vector<float>(numParticles * 4, 0.)),
      pVelArray(std::vector<float>(numParticles * 4, 0.)),
      pColArray(std::vector<float>(numParticles * 4, 0.)), swap(true) {
  for (int i = 0; i < numParticles; i++) {
    particlePos[i] = Vector3(((float)rand()) / RAND_MAX - 0.5,
                             ((float)rand()) / RAND_MAX - 0.5,
                             ((float)rand()) / RAND_MAX * 4);
    particleVel[i] = Vector3((((float)rand()) / RAND_MAX - 0.5) * 3,
                             (((float)rand()) / RAND_MAX - 0.5) * 3,
                             (((float)rand()) / RAND_MAX * 3) - 6.5);
    for (int x = 0; x < resolution.x() - 1; x++) {
      for (int y = 0; y < resolution.y() - 1; y++) {
        for (int z = 0; z < resolution.z() - 1; z++) {
          fVel.set(Vector3i(x, y, z),
                   Vector3(((float)rand()) / RAND_MAX - 0.01,
                           ((float)rand()) / RAND_MAX - 0.01,
                           ((float)rand()) / RAND_MAX - 0.01));
          fTemp.set(Vector3i(x, y, z), ((float)rand()) / RAND_MAX * 20.);
        }
      }
    }
  }
}

SmokeSimulation::~SmokeSimulation() {}

void SmokeSimulation::step(float dt) {
  for (int i = 0; i < num_particles; i++) {
    particleVel[i] = particleVel[i] + Vector3(0., 0., 0.05);
    particlePos[i] = particlePos[i] + particleVel[i] * dt;
  }

  // this->advect_vel(dt);
  // // add external velocities
  // // project(?)
  // this->advect_temp(dt);
  // this->advect_density(dt);
  //
  // // advect rendered particles
  // this->advect_particles(dt);
  // swap = !swap;
}

void SmokeSimulation::advect_density(float dt) {
  // TODO: use an instance variable to reduce memory allocations
  Vector3 cellsize =
      Vector3(size.x() / resolution.x(), size.y() / resolution.y(),
              size.z() / resolution.z());

  Field &oldDensity = swap ? fDensity : newFDensity;
  Field &newDensity = swap ? newFDensity : fDensity;

  // #pragma omp parallel for
  for (int x = 0; x < resolution.x() - 1; x++) {
    for (int y = 0; y < resolution.y() - 1; y++) {
      for (int z = 0; z < resolution.z() - 1; z++) {
        Vector3 cellCenter =
            Vector3((x + 0.5) * cellsize.x(), (y + 0.5) * cellsize.y(),
                    (z + 0.5) * cellsize.z());
        Vector3 posVel = this->fVel.interp(cellCenter);
        Vector3 midpoint = cellCenter - (posVel * dt / 2);
        // TODO: is this supposed to be multiplied by dt?
        Vector3 oldpoint = cellCenter - this->fVel.interp(midpoint);
        newDensity.set(Vector3i(x, y, z), oldDensity.interp(oldpoint));
      }
    }
  }
}

void SmokeSimulation::advect_temp(float dt) {
  Vector3 cellsize =
      Vector3(size.x() / resolution.x(), size.y() / resolution.y(),
              size.z() / resolution.z());

  Field &oldTemp = swap ? fTemp : newFTemp;
  Field &newTemp = swap ? newFTemp : fTemp;

  // #pragma omp parallel for
  for (int x = 0; x < resolution.x() - 1; x++) {
    for (int y = 0; y < resolution.y() - 1; y++) {
      for (int z = 0; z < resolution.z() - 1; z++) {
        Vector3 cellCenter =
            Vector3((x + 0.5) * cellsize.x(), (y + 0.5) * cellsize.y(),
                    (z + 0.5) * cellsize.z());
        Vector3 posVel = this->fVel.interp(cellCenter);
        Vector3 midpoint = cellCenter - (posVel * dt / 2);
        // TODO: is this supposed to be multiplied by dt?
        Vector3 oldpoint = cellCenter - this->fVel.interp(midpoint);
        newTemp.set(Vector3i(x, y, z), oldTemp.interp(oldpoint));
      }
    }
  }
}

void SmokeSimulation::advect_vel(float dt) {
  Vector3 cellsize =
      Vector3(size.x() / resolution.x(), size.y() / resolution.y(),
              size.z() / resolution.z());

  VectorField &oldVel = swap ? fVel : newFVel;
  VectorField &newVel = swap ? newFVel : fVel;

  // #pragma omp parallel for
  for (int x = 0; x < resolution.x() - 1; x++) {
    for (int y = 0; y < resolution.y() - 1; y++) {
      for (int z = 0; z < resolution.z() - 1; z++) {
        Vector3 xFaceCenter =
            Vector3(x * cellsize.x(), (y + 0.5) * cellsize.y(),
                    (z + 0.5) * cellsize.z());
        Vector3 yFaceCenter =
            Vector3((x + 0.5) * cellsize.x(), y * cellsize.y(),
                    (z + 0.5) * cellsize.z());
        Vector3 zFaceCenter =
            Vector3((x + 0.5) * cellsize.x(), (y + 0.5) * cellsize.y(),
                    z * cellsize.z());

        Vector3 xFaceMid = xFaceCenter - oldVel.interp(xFaceCenter);
        Vector3 yFaceMid = yFaceCenter - oldVel.interp(yFaceCenter);
        Vector3 zFaceMid = zFaceCenter - oldVel.interp(zFaceCenter);
        // newFVel.set(Vector3i(x, y, z), Vector3(fVel.x.data_grid[x][y][z],
        //                                        fVel.y.data_grid[x][y][z],
        //                                        fVel.z.data_grid[x][y][z]));

        newVel.set(
            Vector3i(x, y, z),
            Vector3(
                oldVel.x.interp(xFaceCenter - oldVel.interp(xFaceMid) * dt),
                oldVel.y.interp(xFaceCenter - oldVel.interp(xFaceMid) * dt),
                oldVel.z.interp(xFaceCenter - oldVel.interp(xFaceMid) * dt)));
      }
    }
  }
}

// Advect the particles themselves based on the fields
void SmokeSimulation::advect_particles(float dt) {
  // #pragma omp parallel for
  // std::cout << particlePos.size() << std::endl;

  for (int i = 0; i < particlePos.size(); i++) {

    Vector3 cur_pos = particlePos[i];
    Vector3 cur_vel = fVel.interp(cur_pos);
    Vector3 next_pos = cur_pos + cur_vel * dt; // Eulerian motion calculation
    Vector3 next_pos_clipped =
        fVel.clipped(next_pos); // Not related to velocity specifically

    Vector3 next_vel = fVel.interp(next_pos);
    // Vector3 next_vel_clipped = fVel.interp(next_pos_clipped);
    Vector3 average_vel = (cur_vel + next_vel) / 2.0;
    // // Like a little moving average of velocities
    // Vector3 better_next_pos = cur_pos + average_vel * dt;
    // Vector3 better_next_pos_clipped = fVel.clipped(better_next_pos);

    particlePos[i] = next_pos;

    particleVel[i] = average_vel;
  }
}
