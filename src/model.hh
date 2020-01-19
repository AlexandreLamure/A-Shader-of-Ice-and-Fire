#pragma once

#include <optional>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.hh"
#include "light.hh"

class Model {
protected:
    std::string directory;
    std::vector<Texture> textures_loaded;
    GLuint draw_mode;

    void process_node(aiNode *node, const aiScene *scene);
    Mesh process_mesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> load_material_textures(aiMaterial *mat, aiTextureType type, const std::string& type_name);


public:
    std::vector<Mesh> meshes;

    Model(const std::string& path, GLuint draw_mode);
    void draw(Program program, std::vector<GLuint>* other_textures);
    static unsigned int texture_from_file(const char *path, const std::string& directory);
};

class LightModel : public Model {
public:
    PointLight point_light;

    LightModel(const std::string& path, GLuint draw_mode, Light& light);
};