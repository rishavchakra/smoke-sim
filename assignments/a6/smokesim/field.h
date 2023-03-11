#pragma once

#include "Common.h"
class Field {
  float ***data;
  Vector3i resolution;

public:
  Field(Vector3i resolution);
  ~Field();
};
