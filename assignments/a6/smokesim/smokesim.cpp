#include "smokesim.h"
#include "field.h"
#include "glad.h"

SmokeSimulation::SmokeSimulation(Vector3i resolution) {
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

  Field fDensity = Field(resolution);
  Field fVelocity = Field(resolution);
  Field fTemperature = Field(resolution);
  this->resolution = resolution;
}

SmokeSimulation::~SmokeSimulation() {}

void SmokeSimulation::step(float dt) {}
