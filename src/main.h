#pragma once

#include "Common.h"
#include "Driver.h"
#include "OpenGLCommon.h"
#include "OpenGLMarkerObjects.h"
#include "OpenGLMesh.h"
#include "OpenGLParticles.h"
#include "OpenGLViewer.h"
#include "OpenGLWindow.h"
#include "Particles.h"
#include "TinyObjLoader.h"

/**
 * A Vertex shader + fragment shader pipeline
 */
struct ShaderPaths {
  std::string vertShader;
  std::string fragShader;
  std::string name;
};

struct TexturePaths {
  std::string texture_file_name;
  std::string texture_name;
};

struct MeshPaths {
  std::string mesh_file_name;
  std::string mesh_name;
};

struct DataPaths {
  std::vector<ShaderPaths> shaders;
  std::vector<TexturePaths> textures;
  std::vector<MeshPaths> meshes;

  void addShaderPath(ShaderPaths shader) { shaders.push_back(shader); }
  void addTexturePath(TexturePaths texture) { textures.push_back(texture); }
  void addMeshPath(MeshPaths mesh) { meshes.push_back(mesh); }
};

class TechProjDriver : public Driver, public OpenGLViewer {
  using Base = Driver;

public:
  Array<TriangleMesh<3> *> triangle_meshes;

  virtual void Initialize() { OpenGLViewer::Initialize(); }

  virtual void Run() { OpenGLViewer::Run(); }

  virtual void Initialize_Data();

  void Init_Lighting();
  void Init_Shaders();
  void Init_Textures();
  void Init_Background();
};
