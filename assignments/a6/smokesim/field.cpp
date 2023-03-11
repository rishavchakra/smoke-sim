#include "field.h"
#include "Common.h"

Field::Field(Vector3i resolution) : resolution(resolution) {
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
