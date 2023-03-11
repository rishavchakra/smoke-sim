/**
 * Class defining a smoke particle
 */

#pragma once
#include "Common.h"

struct Particle {
  Vector3 pos;
  Vector3 vel;

  Particle() {
    pos = Vector3(0, 0, 0);
    vel = Vector3(0, 0, 0);
  }
};
