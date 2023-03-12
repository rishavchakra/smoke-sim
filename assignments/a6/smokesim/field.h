#pragma once

#include "Common.h"
struct Field {
  std::vector<std::vector<std::vector<float>>> data_grid;
  Vector3i resolution;
  Vector3 size;

  Field(Vector3 size, Vector3i resolution);
  ~Field();

  float interp(Vector3 pos);
  Vector3 clipped(Vector3 pos);
  void set(Vector3i inds, float val);
};

struct VectorField {
  Field x;
  Field y;
  Field z;

  VectorField(Vector3 size, Vector3i resolution);

  Vector3 interp(Vector3 pos);
  Vector3 clipped(Vector3 pos);
  void set(Vector3i inds, Vector3 val);
};
