// #####################################################################
//  Main
//  Dartmouth COSC 77/177 Computer Graphics, starter code
//  Contact: Bo Zhu (bo.zhu@dartmouth.edu)
//  Edited by Eric Lu, Rishav Chakravarty, Camden Hao
// #####################################################################
#include "./smokesim/particle_renderer.h"
#include "./smokesim/smokesim.h"
#include "Common.h"
#include "Driver.h"
#include "OpenGLCommon.h"
#include "OpenGLMarkerObjects.h"
#include "OpenGLMesh.h"
#include "OpenGLViewer.h"
#include "OpenGLWindow.h"
#include "TinyObjLoader.h"
#include <algorithm>
#include <iostream>
#include <random>
#include <unordered_set>
#include <vector>

#ifndef __Main_cpp__
#define __Main_cpp__

#ifdef __APPLE__
#define CLOCKS_PER_SEC 100000
#define MaxParticles 100000
#endif

class FinalProjectDriver : public Driver, public OpenGLViewer {
  using Base = Driver;
  std::vector<OpenGLTriangleMesh *> mesh_object_array;
  std::vector<OpenGLTriangleMesh *> smoke_particle_array;
  SmokeSimulation *smoke_sim;
  ParticleRenderer *particle_renderer;
  std::vector<SmokeSimulation *> particle_renderer_array;
  clock_t startTime;

public:
  virtual void Initialize() {
    draw_bk =
        false; ////turn off the default background and use the customized one
    draw_axes = true; ////if you don't like the axes, turn them off!
    startTime = clock();

    OpenGLViewer::Initialize();
  }

  ////This function adds a mesh object from an obj file
  int Add_Obj_Mesh_Object(std::string obj_file_name) {
    auto mesh_obj = Add_Interactive_Object<OpenGLTriangleMesh>();

    Array<std::shared_ptr<TriangleMesh<3>>> meshes;
    Obj::Read_From_Obj_File_Discrete_Triangles(obj_file_name, meshes);
    mesh_obj->mesh = *meshes[0];
    std::cout << "load tri_mesh from obj file, #vtx: "
              << mesh_obj->mesh.Vertices().size()
              << ", #ele: " << mesh_obj->mesh.Elements().size() << std::endl;

    mesh_object_array.push_back(mesh_obj);
    return (int)mesh_object_array.size() - 1;
  }

  void Add_Shaders() {
    ////format: vertex shader name, fragment shader name, shader name
    OpenGLShaderLibrary::Instance()->Add_Shader_From_File(
        "background.vert", "background.frag", "background");

    ////SHADOW TODO: uncomment next three lines to import shadow shaders
    // OpenGLShaderLibrary::Instance()->Add_Shader_From_File("object_1_shadow.vert","object_1_shadow.frag","object_1_shadow");
    // OpenGLShaderLibrary::Instance()->Add_Shader_From_File("sky_sphere_shadow.vert","sky_sphere_shadow.frag","sky_sphere_shadow");
    // OpenGLShaderLibrary::Instance()->Add_Shader_From_File("object_3_shadow.vert","object_3_shadow.frag","object_3_shadow");

    ////SHADOW TODO: comment out next three lines
    OpenGLShaderLibrary::Instance()->Add_Shader_From_File(
        "normal_mapping.vert", "normal_mapping.frag", "object_1");
    OpenGLShaderLibrary::Instance()->Add_Shader_From_File(
        "sky_sphere.vert", "sky_sphere.frag", "sky_sphere");
    OpenGLShaderLibrary::Instance()->Add_Shader_From_File(
        "object_3.vert", "object_3.frag", "object_3");

    OpenGLShaderLibrary::Instance()->Add_Shader_From_File("wave.vert",
                                                          "wave.frag", "wave");

    OpenGLShaderLibrary::Instance()->Add_Shader_From_File("volcano.vert","volcano.frag","volcano");

    OpenGLShaderLibrary::Instance()->Add_Shader_From_File("particle.vert","particle.frag","particle");
    
    OpenGLShaderLibrary::Instance()->Add_Shader_From_File("plane_body.vert","plane_body.frag","plane_body");
		
    OpenGLShaderLibrary::Instance()->Add_Shader_From_File("plane_wing.vert","plane_wing.frag","plane_wing");
  }

  void Add_Textures() {
    ////format: image name, texture name
    OpenGLTextureLibrary::Instance()->Add_Texture_From_File(
        "airplane_wings.jpg", "plane_wings_albedo");
    OpenGLTextureLibrary::Instance()->Add_Texture_From_File(
        "airplane_wings.jpg", "plane_wings_normal");
    OpenGLTextureLibrary::Instance()->Add_Texture_From_File("smoke.png",
                                                            "smoke_albedo");
    OpenGLTextureLibrary::Instance()->Add_Texture_From_File("smoke.png",
                                                            "smoke_normal");
    OpenGLTextureLibrary::Instance()->Add_Texture_From_File(
        "airplane_body.jpg", "plane_body_albedo");
    OpenGLTextureLibrary::Instance()->Add_Texture_From_File(
        "airplane_body.jpg", "plane_body_normal");
    OpenGLTextureLibrary::Instance()->Add_Texture_From_File("earth_albedo.png",
                                                            "object_1_albedo");
    OpenGLTextureLibrary::Instance()->Add_Texture_From_File("earth_normal.png",
                                                            "object_1_normal");
    OpenGLTextureLibrary::Instance()->Add_Texture_From_File("earth_albedo.png",
                                                            "object_3_albedo");
    OpenGLTextureLibrary::Instance()->Add_Texture_From_File("earth_normal.png",
                                                            "object_3_normal");
    OpenGLTextureLibrary::Instance()->Add_Texture_From_File(
        "sky_sphere_albedo.png", "sky_sphere_albedo");
    // OpenGLTextureLibrary::Instance()->Add_Texture_From_File("earth_albedo.png",
    // "sky_sphere_normal");

    OpenGLTextureLibrary::Instance()->Add_Texture_From_File("volcano_albedo.png", "volcano_albedo");
		OpenGLTextureLibrary::Instance()->Add_Texture_From_File("volcano_normal.png", "volcano_normal");
  }

  void Add_Background() {
    OpenGLBackground *opengl_background =
        Add_Interactive_Object<OpenGLBackground>();
    opengl_background->shader_name = "background";
    opengl_background->Initialize();
  }

  ////this is an example of adding a mesh object read from obj file
  int Plane_Object() {
    auto mesh_obj = Add_Interactive_Object<OpenGLTriangleMesh>();

    ////read mesh file
    std::string obj_file_name = "airplane.obj";
    Array<std::shared_ptr<TriangleMesh<3>>> meshes;
    Obj::Read_From_Obj_File_Discrete_Triangles(obj_file_name, meshes);
    mesh_obj->mesh = *meshes[0];

    std::vector<Vector3> &vertices = mesh_obj->mesh.Vertices();
    int vn = (int)vertices.size();
    for (int i = 0; i < vn; i++) {
      vertices[i][0] *= .5;
      vertices[i][1] *= .5;
      vertices[i][2] *= -.5;
      vertices[i][2] -= 1000.;
    }

    ////This is an example of creating a 4x4 matrix for GLSL shaders. Notice
    /// that the matrix is column-major (instead of row-major!) /The code for
    /// passing the matrix to the shader is in OpenGLMesh.h
    mesh_obj->model_matrix = glm::mat4(
        1.f, 0.f, 0.f, 0.f, ////column 0
        0.f, 1.f, 0.f, 0.f, ////column 1
        0.f, 0.f, 1.f, 0.f, ////column 2
        0.f, 1.f, 0.f,
        1.f); ////column 3	////set the translation in the last column

    ////set up shader
    // mesh_obj->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("object_1_shadow"));//Shadow
    // TODO: uncomment this line and comment next line to use shadow shader
		mesh_obj->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("plane_body"));

    ////set up texture
    mesh_obj->Add_Texture(
        "tex_albedo", OpenGLTextureLibrary::Get_Texture("plane_body_albedo"));
    mesh_obj->Add_Texture(
        "tex_normal", OpenGLTextureLibrary::Get_Texture("plane_body_normal"));
    Set_Polygon_Mode(mesh_obj, PolygonMode::Fill);
    Set_Shading_Mode(
        mesh_obj,
        ShadingMode::Texture); // SHADOW TODO: set Shading Mode to Shadow

    ////initialize
    mesh_obj->Set_Data_Refreshed();
    mesh_obj->Initialize();
    mesh_object_array.push_back(mesh_obj);
    return (int)mesh_object_array.size() - 1;
  }

  int Plane_Wings_Object() {
    auto mesh_obj = Add_Interactive_Object<OpenGLTriangleMesh>();

    ////read mesh file
    std::string obj_file_name = "airplane.obj";
    Array<std::shared_ptr<TriangleMesh<3>>> meshes;
    Obj::Read_From_Obj_File_Discrete_Triangles(obj_file_name, meshes);
    mesh_obj->mesh = *meshes[1];

    ////This is an example showing how to access and modify the values of
    /// vertices on the CPU end.
    std::vector<Vector3> &vertices = mesh_obj->mesh.Vertices();
    int vn = (int)vertices.size();
    for (int i = 0; i < vn; i++) {
      vertices[i][0] *= .5;
      vertices[i][1] *= .5;
      vertices[i][2] *= -.5;
      vertices[i][2] -= 1000.;
    }

    ////This is an example of creating a 4x4 matrix for GLSL shaders. Notice
    /// that the matrix is column-major (instead of row-major!) /The code for
    /// passing the matrix to the shader is in OpenGLMesh.h
    mesh_obj->model_matrix = glm::mat4(
        1.f, 0.f, 0.f, 0.f, ////column 0
        0.f, 1.f, 0.f, 0.f, ////column 1
        0.f, 0.f, 1.f, 0.f, ////column 2
        0.f, 1.f, 0.f,
        1.f); ////column 3	////set the translation in the last column

    ////set up shader
    // mesh_obj->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("object_1_shadow"));//Shadow
    // TODO: uncomment this line and comment next line to use shadow shader
		mesh_obj->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("plane_wing"));

    ////set up texture
    mesh_obj->Add_Texture(
        "tex_albedo", OpenGLTextureLibrary::Get_Texture("plane_wings_albedo"));
    mesh_obj->Add_Texture(
        "tex_normal", OpenGLTextureLibrary::Get_Texture("plane_wings_normal"));
    Set_Polygon_Mode(mesh_obj, PolygonMode::Fill);
    Set_Shading_Mode(
        mesh_obj,
        ShadingMode::Texture); // SHADOW TODO: set Shading Mode to Shadow

    ////initialize
    mesh_obj->Set_Data_Refreshed();
    mesh_obj->Initialize();
    mesh_object_array.push_back(mesh_obj);
    return (int)mesh_object_array.size() - 1;
  }

  int Add_Sky_Sphere() {
    auto mesh_obj = Add_Interactive_Object<OpenGLTriangleMesh>();

    real radius = 10000.;
    Initialize_Sphere_Mesh(radius, &mesh_obj->mesh,
                           3); ////add a sphere with radius=1. if the obj file
                               /// name is not specified

    // 	////set up shader
    // 	//mesh_obj->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("sky_sphere_shadow"));//Shadow
    // TODO: uncomment this line and comment next line to use shadow shader
    mesh_obj->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("sky_sphere"));

    // 	////set up texture
    mesh_obj->Add_Texture(
        "tex_albedo", OpenGLTextureLibrary::Get_Texture("sky_sphere_albedo"));
    // mesh_obj->Add_Texture("tex_normal",
    // OpenGLTextureLibrary::Get_Texture("sky_sphere_normal")); // uv normals
    // added manually

    Set_Polygon_Mode(mesh_obj, PolygonMode::Fill);
    Set_Shading_Mode(
        mesh_obj,
        ShadingMode::Texture); // SHADOW TODO: Set Shading Mode to Shadow

    // 	////initialize
    mesh_obj->Set_Data_Refreshed();
    mesh_obj->Initialize();
    mesh_object_array.push_back(mesh_obj);
    return (int)mesh_object_array.size() - 1;
  }

  // ////this is an example of adding an object with manually created triangles
  // and vertex attributes int Add_Object_3()
  // {
  // 	auto mesh_obj=Add_Interactive_Object<OpenGLTriangleMesh>();
  // 	auto& mesh=mesh_obj->mesh;

  // 	////vertex position
  // 	std::vector<Vector3>
  // triangle_vertices={Vector3(-1,-1,-1),Vector3(1,-1,-1),Vector3(-1,-1,1),Vector3(1,-1,1)};
  // 	std::vector<Vector3>& vertices=mesh_obj->mesh.Vertices();
  // 	vertices=triangle_vertices;

  // 	////vertex color
  // 	std::vector<Vector4f>& vtx_color=mesh_obj->vtx_color;
  // 	vtx_color={Vector4f(1.f,0.f,0.f,1.f),Vector4f(0.f,1.f,0.f,1.f),Vector4f(0.f,0.f,1.f,1.f),Vector4f(1.f,1.f,0.f,1.f)};

  // 	////vertex normal
  // 	std::vector<Vector3>& vtx_normal=mesh_obj->vtx_normal;
  // 	vtx_normal={Vector3(0.,1.,0.),Vector3(0.,1.,0.),Vector3(0.,1.,0.),Vector3(0.,1.,0.)};

  // 	////vertex uv
  // 	std::vector<Vector2>& uv=mesh_obj->mesh.Uvs();
  // 	uv={Vector2(0.,0.),Vector2(1.,0.),Vector2(0.,1.),Vector2(1.,1.)};

  // 	////mesh elements
  // 	std::vector<Vector3i>& elements=mesh_obj->mesh.Elements();
  // 	elements={Vector3i(0,1,3),Vector3i(0,3,2)};

  // 	////set up shader
  // 	//mesh_obj->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("object_3_shadow"));//Shadow
  // TODO: uncomment this line and comment next line to use shadow shader
  // 	mesh_obj->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("object_3"));

  // 	////set up texture
  // 	mesh_obj->Add_Texture("tex_albedo",
  // OpenGLTextureLibrary::Get_Texture("object_3_albedo"));
  // 	mesh_obj->Add_Texture("tex_normal",
  // OpenGLTextureLibrary::Get_Texture("object_3_normal"));
  // 	Set_Polygon_Mode(mesh_obj,PolygonMode::Fill);
  // 	Set_Shading_Mode(mesh_obj,ShadingMode::Texture);//SHADOW TODO: Set
  // Shading Mode to Shadow

  // 	////initialize
  // 	mesh_obj->Set_Data_Refreshed();
  // 	mesh_obj->Initialize();
  // 	mesh_object_array.push_back(mesh_obj);
  // 	return (int)mesh_object_array.size()-1;
  // }

  int Add_Object_Wave() {
    ////add the plane mesh object
    int obj_idx = Add_Obj_Mesh_Object("wave.obj");
    auto wave_obj = mesh_object_array[obj_idx];
    wave_obj->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("wave"));

    wave_obj->Add_Texture("tex_albedo", OpenGLTextureLibrary::Get_Texture("sky_sphere_albedo"));

    Set_Polygon_Mode(wave_obj, PolygonMode::Fill);
    Set_Shading_Mode(wave_obj, ShadingMode::Texture);
    wave_obj->Set_Data_Refreshed();
    wave_obj->Initialize();
  }

  int Add_Volcano() {
		////add the plane mesh object
		int obj_idx = Add_Obj_Mesh_Object("volcano.obj");
		auto volcano_obj = mesh_object_array[obj_idx];

		////This is an example showing how to access and modify the values of vertices on the CPU end.
		std::vector<Vector3>& vertices=volcano_obj->mesh.Vertices();
		int vn=(int)vertices.size();
		for(int i=0;i<vn;i++){
			float x = vertices[i][0];
			float y = vertices[i][1];
			float z = vertices[i][2];

			float newX = z;
			float newY = y;
			float newZ = x;
			vertices[i] = Vector3(newX, newY, newZ);
		}

		volcano_obj->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("volcano"));

		volcano_obj->Add_Texture("tex_albedo", OpenGLTextureLibrary::Get_Texture("volcano_albedo"));
		volcano_obj->Add_Texture("tex_normal", OpenGLTextureLibrary::Get_Texture("volcano_normal"));
		Set_Polygon_Mode(volcano_obj, PolygonMode::Fill);
		Set_Shading_Mode(volcano_obj, ShadingMode::Texture);
		volcano_obj->Set_Data_Refreshed();
		volcano_obj->Initialize();
		// mesh_object_array.push_back(volcano_obj);
		// return (int)mesh_object_array.size()-1;
	}

  int Add_Object_Skysphere() {
    auto mesh_obj = Add_Interactive_Object<OpenGLTriangleMesh>();

    real radius = 100.;
    Initialize_Sphere_Mesh(radius, &mesh_obj->mesh,
                           3); ////add a sphere with radius=1. if the obj file
                               /// name is not specified

    ////set up shader
    // mesh_obj->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("sky_sphere_shadow"));//Shadow
    // TODO: uncomment this line and comment next line to use shadow shader
    mesh_obj->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("sky_sphere"));

    ////set up texture
    mesh_obj->Add_Texture(
        "tex_albedo", OpenGLTextureLibrary::Get_Texture("sky_sphere_albedo"));
    mesh_obj->Add_Texture(
        "tex_normal", OpenGLTextureLibrary::Get_Texture("sky_sphere_normal"));

    Set_Polygon_Mode(mesh_obj, PolygonMode::Fill);
    Set_Shading_Mode(
        mesh_obj,
        ShadingMode::Texture); // SHADOW TODO: Set Shading Mode to Shadow

    ////initialize
    mesh_obj->Set_Data_Refreshed();
    mesh_obj->Initialize();
    mesh_object_array.push_back(mesh_obj);
    return (int)mesh_object_array.size() - 1;
  }

  int CreateSmokeParticles(std::vector<Vector3> particlePos) {
    for (int i = 0; i < particlePos.size(); i++) {

      auto mesh_obj = Add_Interactive_Object<OpenGLTriangleMesh>();
      auto &mesh = mesh_obj->mesh;

      ////vertex position
      std::vector<Vector3> triangle_vertices = {
          particlePos[i] + Vector3(-1, -1, 1),
          particlePos[i] + Vector3(-1, 1, 1),
          particlePos[i] + Vector3(1, -1, 1),
          particlePos[i] + Vector3(1, 1, 1)};
      std::vector<Vector3> &vertices = mesh_obj->mesh.Vertices();
      vertices = triangle_vertices;

      ////vertex color
      std::vector<Vector4f> &vtx_color = mesh_obj->vtx_color;
      vtx_color = {Vector4f(1.0, 0.647, 0, 1.f), Vector4f(1.0, 0.647, 0, 1.f),
                   Vector4f(1.0, 0.647, 0, 1.f), Vector4f(1.0, 0.647, 0, 1.f)};

      ////vertex normal
      std::vector<Vector3> &vtx_normal = mesh_obj->vtx_normal;
      vtx_normal = {Vector3(0., 1., 0.), Vector3(0., 1., 0.),
                    Vector3(0., 1., 0.), Vector3(0., 1., 0.)};

      ////vertex uv
      std::vector<Vector2> &uv = mesh_obj->mesh.Uvs();
      uv = {Vector2(0., 0.), Vector2(1., 0.), Vector2(0., 1.), Vector2(1., 1.)};

      ////mesh elements
      std::vector<Vector3i> &elements = mesh_obj->mesh.Elements();
      elements = {Vector3i(0, 1, 3), Vector3i(0, 3, 2)};

      ////set up shader
      // mesh_obj->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("object_3_shadow"));//Shadow
      // TODO: uncomment this line and comment next line to use shadow shader
      mesh_obj->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("particle"));

      ////set up texture
      mesh_obj->Add_Texture("tex_albedo",
                            OpenGLTextureLibrary::Get_Texture("smoke_albedo"));
      mesh_obj->Add_Texture("tex_normal",
                            OpenGLTextureLibrary::Get_Texture("smoke_normal"));
      Set_Polygon_Mode(mesh_obj, PolygonMode::Fill);
      Set_Shading_Mode(
          mesh_obj,
          ShadingMode::Texture); // SHADOW TODO: Set Shading Mode to Shadow

      ////initialize
      ////initialize
      mesh_obj->Set_Data_Refreshed();
      mesh_obj->Initialize();

      smoke_particle_array.push_back(mesh_obj);
    }
    return (int)smoke_particle_array.size() - 1;
  }

  int UpdateSmokeParticles(std::vector<Vector3> particlePos) {
    std::cout << smoke_particle_array.size() << std::endl;
    for (int i = 0; i < particlePos.size(); i++) {

      auto mesh_obj = smoke_particle_array[i];
      auto &mesh = mesh_obj->mesh;

      ////vertex position
      std::vector<Vector3> triangle_vertices = {
          particlePos[i] + Vector3(0, -20, -20),
          particlePos[i] + Vector3(0, -20, 20),
          particlePos[i] + Vector3(0, 20, -20),
          particlePos[i] + Vector3(0, 20, 20)};
      std::vector<Vector3> &vertices = mesh_obj->mesh.Vertices();
      vertices = triangle_vertices;
      mesh_obj->Set_Data_Refreshed();
    }
    return (int)smoke_particle_array.size() - 1;
  }

  void Add_Particle_Renderer() {
    smoke_sim =
        new SmokeSimulation(Vector3(10., 10., 10.), Vector3i(10, 10, 10), 1000);
    CreateSmokeParticles(smoke_sim->particlePos);
    SmokeSimulation &ref = *smoke_sim;
    particle_renderer = new ParticleRenderer(ref, 100);
  }

  void Init_Lighting() {
    auto dir_light = OpenGLUbos::Add_Directional_Light(
        glm::vec3(-1.f, -1.f, -1.f));            // Light direction
    dir_light->dif = glm::vec4(.9, .8, .7, 1.0); // diffuse reflection color
    dir_light->Set_Shadow(); ////SHADOW TODO: turn on the shadow by uncommenting
                             /// this line, currenly our base code only supports
                             /// shadow for one light

    // This is an example to add a second directional light
    // auto dir_light_2 =
    // OpenGLUbos::Add_Directional_Light(glm::vec3(1.,1.,1.));//the parameter is
    // light direction dir_light_2->dif = glm::vec4(.9,.9,.9,1.0);
    // dir_light_2->spec = glm::vec4(.2,.2,.2,1.0);

    // This is an example to add a point light
    // auto point_light = OpenGLUbos::Add_Point_Light(glm::vec3(1.,1.,1.));//the
    // parameter is the position of point light point_light->dif =
    // glm::vec4(.9,.8,.7, 1.0);

    // This is an example to add a spot light
    //  auto spot_light =
    //  OpenGLUbos::Add_Spot_Light(glm::vec3(1.,1.,1.),glm::vec3(1.,1.,1.));//first
    //  param: position, second param: direction spot_light->dif =
    //  glm::vec4(.9,.8,.7, 1.0);

    OpenGLUbos::Set_Ambient(glm::vec4(.01f, .01f, .02f, 1.f));
    OpenGLUbos::Update_Lights_Ubo();
  }

  virtual void Initialize_Data() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_MAX);
    Add_Shaders();
    Add_Textures();

    Add_Background();
    Add_Object_Wave();
    Plane_Object();
    Plane_Wings_Object();
    Add_Sky_Sphere();
    Add_Volcano();

    Add_Particle_Renderer();

    Init_Lighting(); ////SHADOW TODO: uncomment this line
  }

  ////Goto next frame
  virtual void Toggle_Next_Frame() {
    for (auto &mesh_obj : mesh_object_array) {
      mesh_obj->setTime(GLfloat(clock() - startTime) / CLOCKS_PER_SEC);
    }
    // for (auto &mesh_obj : smoke_particle_array) {
    //   mesh_obj->setTime(GLfloat(clock() - startTime) / CLOCKS_PER_SEC);
    // }
    smoke_sim->step(1.);
    UpdateSmokeParticles(smoke_sim->particlePos);

    particle_renderer->step();
    // These functions are specific to glDrawArrays*Instanced*.
    // The first parameter is the attribute buffer we're talking about.
    // The second parameter is the "rate at which generic vertex attributes
    // advance when rendering multiple instances"
    // http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
    OpenGLViewer::Toggle_Next_Frame();
  }

  virtual void Run() { OpenGLViewer::Run(); }
};

int main(int argc, char *argv[]) {
  FinalProjectDriver driver;
  driver.Initialize();
  driver.Run();
}

#endif