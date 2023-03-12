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

  // load_shaders("smokesim/particle.vert", "smokesim/particle.frag");
//   load_shaders("object_1.vert", "object_1.frag");
//   compile_shaders();
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

  glVertexAttribDivisor(0, 0); // Vertices buffer: shared so 0 per quad
  glVertexAttribDivisor(1, 1); // Positions buffer: one per quad
  glVertexAttribDivisor(2, 1); // Color buffer: one per quad
}

void ParticleRenderer::unbind_buffers() {
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
}

void ParticleRenderer::draw() {
  // glUseProgram(OpenGLShaderLibrary::Get_Shader("particle"));

  // Actually draw the particles
//   use_shaders();
  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, num_particles);
}

void ParticleRenderer::step() {
  update_buffers();
  bind_buffers();
  draw();
  unbind_buffers();
}

void ParticleRenderer::load_shaders(const char *vertPath,
                                    const char *fragPath) {
  std::string vertexCode;
  std::string fragmentCode;
  std::ifstream vShaderFile;
  std::ifstream fShaderFile;
  // ensure ifstream objects can throw exceptions:
  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    // open files
    vShaderFile.open(vertPath);
    fShaderFile.open(fragPath);
    std::stringstream vShaderStream, fShaderStream;
    // read file's buffer contents into streams
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    // close file handlers
    vShaderFile.close();
    fShaderFile.close();
    // convert stream into string
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
    std::cout << "THIS IS MY VERTEX SHADER BITCH\n" << vertexCode << std::endl;
    std::cout << fragmentCode << std::endl;
  } catch (std::ifstream::failure e) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
  }
  const char *vShaderCode = vertexCode.c_str();
  const char *fShaderCode = fragmentCode.c_str();
  VS = vertexCode.c_str();
  FS = fragmentCode.c_str();
}

void ParticleRenderer::compile_shaders() {
  unsigned int vertex, fragment;
  int success;
  char infoLog[512];

  // vertex Shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &VS, NULL);
  glCompileShader(vertex);
  // print compile errors if any
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  };

  // similiar for Fragment Shader
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &FS, NULL);
  glCompileShader(fragment);
  // print compile errors if any
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  };

  // shader Program
  auto shader_program_id = glCreateProgram();
  glAttachShader(shader_program_id, vertex);
  glAttachShader(shader_program_id, fragment);
  glLinkProgram(shader_program_id);
  // print linking errors if any
  glGetProgramiv(shader_program_id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shader_program_id, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
              << infoLog << std::endl;
  }

  // delete the shaders as they're linked into our program now and no longer
  // necessary
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void ParticleRenderer::use_shaders() { glUseProgram(shader_program_id); }

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
