#pragma once

#include "../common.h"
#include "mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

CLASS_PTR(Model);
class Model
{
public:
    static ModelUPtr Load(const std::string &filename);

    int GetMeshCount() const { return (int)m_meshes.size(); }
    MeshPtr GetMesh(int index) const { return m_meshes[index]; }
    std::vector<glm::vec3>* GetPositions() { return &m_positions; }
    void DeletePositions() { m_positions.clear(); } // Only for Formation, So, should delete after GetPosition
    void Draw(const Program* program) const;

private:
    Model() {}
    bool LoadByAssimp(const std::string &filename);
    void ProcessMesh(aiMesh *mesh, const aiScene *scene);
    void ProcessNode(aiNode *node, const aiScene *scene);

    std::vector<MeshPtr> m_meshes;
    std::vector<MaterialPtr> m_materials;
    
    std::vector<glm::vec3> m_positions;
};