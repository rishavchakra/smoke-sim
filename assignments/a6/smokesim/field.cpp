#include "field.h"
#include "Common.h"
#include "detail/func_common.hpp"

Field::Field(Vector3 size, Vector3i resolution)
    : size(size), resolution(resolution) {
  this->data = new float **[resolution.x()];
  for (int x = 0; x < this->resolution.x(); x++) {
    this->data[x] = new float *[resolution.y()];
    for (int y = 0; y < this->resolution.y(); y++) {
      this->data[x][y] = new float[resolution.z()];
    }
  }
}

Field::~Field() {
  for (int x = 0; x < this->resolution.x(); x++) {
    for (int y = 0; y < this->resolution.y(); y++) {
      delete[] this->data[x][y];
    }
    delete[] this->data[x];
  }
}

float Field::interp(Vector3 pos) {
  Vector3 cellsize =
      Vector3(size.x() / resolution.x(), size.y() / resolution.y(),
              size.z() / resolution.z());
  Vector3 proportion =
      Vector3(pos.x() / size.x(), pos.y() / size.y(), pos.z() / size.z());
  // TODO: maybe add bounds checking
  Vector3i indices = Vector3i(floor(proportion.x() * resolution.x()),
                              floor(proportion.y() * resolution.y()),
                              floor(proportion.z() * resolution.z()));

  Vector3 rem = Vector3((pos.x() - indices.x()), floor(pos.y() - indices.y()),
                        floor(pos.z() - indices.z()));

  // Trilinear interpolation
  return mix(
      mix(mix(data[indices.x()][indices.y()][indices.z()],
              data[indices.x()][indices.y()][indices.z() + 1], rem.z()),
          mix(data[indices.x()][indices.y() + 1][indices.z()],
              data[indices.x()][indices.y() + 1][indices.z() + 1], rem.z()),
          rem.y()),
      mix(mix(data[indices.x() + 1][indices.y()][indices.z()],
              data[indices.x()][indices.y()][indices.z() + 1], rem.z()),
          mix(data[indices.x() + 1][indices.y() + 1][indices.z()],
              data[indices.x()][indices.y() + 1][indices.z() + 1], rem.z()),
          rem.y()),
      rem.x());
}

Vector3 Field::clipped(Vector3 pos) { return pos; }

void Field::set(Vector3i inds, float val) {
  this->data[inds.x()][inds.y()][inds.z()] = val;
}

VectorField::VectorField(Vector3 size, Vector3i resolution)
    : x(Field(size, resolution)), y(Field(size, resolution)),
      z(Field(size, resolution)) {}

Vector3 VectorField::interp(Vector3 pos) {
  return Vector3(this->x.interp(pos), this->y.interp(pos), this->z.interp(pos));
}

void VectorField::set(Vector3i inds, Vector3 val) {
  this->x.set(inds, val.x());
  this->y.set(inds, val.y());
  this->z.set(inds, val.z());
}

Vector3 VectorField::clipped(Vector3 pos) { return pos; }
