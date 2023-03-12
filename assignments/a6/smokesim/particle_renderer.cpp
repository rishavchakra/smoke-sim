#include "particle_renderer.h"
#include "Common.h"
#include "Driver.h"
#include "OpenGLCommon.h"
#include "OpenGLMarkerObjects.h"
#include "OpenGLMesh.h"
#include "OpenGLViewer.h"
#include "OpenGLWindow.h"
#include "TinyObjLoader.h"
#include "glad.h"
#include "smokesim.h"

ParticleRenderer::ParticleRenderer(SmokeSimulation &smoke_sim,
                                   int num_particles)
    : num_particles(num_particles), smoke_sim(smoke_sim) {
  const GLfloat billboard_vert_buf_data[12] = {
      -10., -10., 0.0f, 10., -10., 0.0f, -10., 10., 0.0f, 10., 10., 0.0f};
  glGenBuffers(1, &billboard_vert_buf);
  glBindBuffer(GL_ARRAY_BUFFER, billboard_vert_buf);
  glBufferData(GL_ARRAY_BUFFER, sizeof(billboard_vert_buf_data),
               billboard_vert_buf_data, GL_STATIC_DRAW);

  glGenBuffers(1, &particle_positions_buf);
  glBindBuffer(GL_ARRAY_BUFFER, particle_positions_buf);
  glBufferData(GL_ARRAY_BUFFER, num_particles * 4 * sizeof(GLfloat), NULL,
               GL_STREAM_DRAW);

  glGenBuffers(1, &particle_color_buf);
  glBindBuffer(GL_ARRAY_BUFFER, particle_color_buf);
  glBufferData(GL_ARRAY_BUFFER, num_particles * 4 * sizeof(GLfloat), NULL,
               GL_STREAM_DRAW);

  // const std::string vert_shader = "./assignments/a6/smokesim/smoke.vert";
  // const std::string frag_shader = "./assignments/a6/smokesim/smoke.frag";
}

void ParticleRenderer::update_buffers() {
  glBindBuffer(GL_ARRAY_BUFFER, particle_positions_buf);
  // GL buffer orphaning
  glBufferData(GL_ARRAY_BUFFER, num_particles * 4 * sizeof(GLfloat), NULL,
               GL_STREAM_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, num_particles * 4 * sizeof(GLfloat),
                  &smoke_sim.pPosArray[0]);

  glBindBuffer(GL_ARRAY_BUFFER, particle_color_buf);
  // GL buffer orphaning
  glBufferData(GL_ARRAY_BUFFER, num_particles * 4 * sizeof(GLfloat), NULL,
               GL_STREAM_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, num_particles * 4 * sizeof(GLfloat),
                  &smoke_sim.pColArray[0]);
}

void ParticleRenderer::bind_buffers() {
  // First buffer: vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, billboard_vert_buf);
  glVertexAttribPointer(
      0,        // attribute (needs to match the layout in the shader)
      3,        // size (each vertex takes up 3 spaces for XYZ)
      GL_FLOAT, // type
      GL_FALSE, // I have no idea.
      0,        // stride
      (void *)0 // array buffer offset
  );

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, particle_positions_buf);
  glVertexAttribPointer(
      1,        // attribute (needs to match the layout in the shader)
      4,        // size (XYZ + size)
      GL_FLOAT, // type
      GL_FALSE, // I have no idea.
      0,        // stride
      (void *)0 // array buffer offset
  );

  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, particle_color_buf);
  glVertexAttribPointer(
      2,        // attribute (needs to match the layout in the shader)
      4,        // size (XYZ + size)
      GL_FLOAT, // type
      GL_FALSE, // I have no idea.
      0,        // stride
      (void *)0 // array buffer offset
  );
}

void ParticleRenderer::unbind_buffers() {
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
}

void ParticleRenderer::draw() {
  // glUseProgram(OpenGLShaderLibrary::Get_Shader("particle"));
  glVertexAttribDivisor(0, 0); // Vertices buffer: shared so 0 per quad
  glVertexAttribDivisor(1, 1); // Positions buffer: one per quad
  glVertexAttribDivisor(2, 1); // Color buffer: one per quad

  // Actually draw the particles
  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, num_particles);
}

void ParticleRenderer::step() {
  update_buffers();
  bind_buffers();
  draw();
  unbind_buffers();
}

//////// Particle Rendering with Bo Zhu's code

ParticleRender::ParticleRender(SmokeSimulation &smoke_sim, int num_particles)
    : smoke_sim(smoke_sim), num_particles(num_particles),
      points(OpenGLPoints()) {
  points.Initialize();
  points.Set_Data_Pointers(&smoke_sim.particlePos);
  points.Update_Data_To_Render();
}

void ParticleRender::draw() { points.Display(); }

void ParticleRender::update_buffers() {
  points.Set_Data_Pointers(&smoke_sim.particlePos);
  points.Update_Data_To_Render();
}

void ParticleRender::step() {
  update_buffers();
  draw();
}
