#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.hh"

class Model {
private:
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;

    void process_node(aiNode *node, const aiScene *scene);
    Mesh process_mesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> load_material_textures(aiMaterial *mat, aiTextureType type, std::string type_name);
    unsigned int texture_from_file(const char *path, const std::string &directory);


public:
    Model(std::string path);

    void draw(Program program);
    void draw(Program program, GLuint tex_buffer); // Draw from FBO
};