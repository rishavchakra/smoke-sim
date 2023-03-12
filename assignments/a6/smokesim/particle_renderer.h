#pragma once

#include "glad.h"
#include "smokesim.h"
class ParticleRenderer {
  Vector3 size;
  Vector3i resolution;
  int num_particles;
  constexpr static const GLfloat billboard_vert_buf_data[12] = {
      -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f,
      -0.5f, 0.5f,  0.0f, 0.5f, 0.5f,  0.0f};
  GLuint billboard_vert_buf;
  GLuint particle_positions_buf;

  public: 
    SmokeSimulation &smoke_sim;
    ParticleRenderer(SmokeSimulation &smoke_sim, int num_particles);
    void draw(); 
    void update_buffers();
    void step();

      // : num_particles(num_particles), smoke_sim(smoke_sim) {
  //   glGenBuffers(1, &billboard_vert_buf);
  //   glBindBuffer(GL_ARRAY_BUFFER, billboard_vert_buf);
  //   glBufferData(GL_ARRAY_BUFFER, sizeof(billboard_vert_buf_data),
  //                billboard_vert_buf_data, GL_STATIC_DRAW);

  //   glGenBuffers(1, &particle_positions_buf);
  //   glBindBuffer(GL_ARRAY_BUFFER, particle_positions_buf);
  //   glBufferData(GL_ARRAY_BUFFER, num_particles * 4 * sizeof(GLfloat), NULL,
  //                GL_STREAM_DRAW);
  // }

  protected: 
    void bind_buffers(); 
  // void update_buffers() {
  //   glBindBuffer(GL_ARRAY_BUFFER, particle_positions_buf);
  //   // GL buffer orphaning
  //   glBufferData(GL_ARRAY_BUFFER, num_particles * 4 * sizeof(GLfloat), NULL,
  //                GL_STREAM_DRAW);
  //   glBufferSubData(GL_ARRAY_BUFFER, 0, num_particles * 4 * sizeof(GLfloat),
  //                   &smoke_sim.particlePos[0]);
  // }

  // void bind_buffers() {
  //   // First buffer: vertices
  //   glEnableVertexAttribArray(0);
  //   glBindBuffer(GL_ARRAY_BUFFER, billboard_vert_buf);
  //   glVertexAttribPointer(
  //       0,        // attribute (needs to match the layout in the shader)
  //       3,        // size (each vertex takes up 3 spaces for XYZ)
  //       GL_FLOAT, // type
  //       GL_FALSE, // I have no idea.
  //       0,        // stride
  //       (void *)0 // array buffer offset
  //   );

  //   glEnableVertexAttribArray(1);
  //   glBindBuffer(GL_ARRAY_BUFFER, billboard_vert_buf);
  //   glVertexAttribPointer(
  //       1,        // attribute (needs to match the layout in the shader)
  //       4,        // size (XYZ + size)
  //       GL_FLOAT, // type
  //       GL_FALSE, // I have no idea.
  //       0,        // stride
  //       (void *)0 // array buffer offset
  //   );
  // }

    // Actually draw the particles

};
