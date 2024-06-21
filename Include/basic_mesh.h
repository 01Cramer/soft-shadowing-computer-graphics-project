#ifndef BASIC_MESH_H
#define BASIC_MESH_H

#include <map>
#include <vector>
#include <GL/glew.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "util.h"
#include "math_3d.h"
#include "texture.h"
#include "world_transform.h"
#include "material.h"
#include "common.h"

class BasicMesh : public MeshCommon
{
public:
    BasicMesh(){};
    ~BasicMesh();

    bool LoadMesh(const std::string& Filename);

    void Render(IRenderCallbacks* pRenderCallbacks = NULL);

    const Material& GetMaterial();

protected:

    void Clear();
    virtual void ReserveSpace(unsigned int NumVertices, unsigned int NumIndices);
    virtual void InitSingleMesh(unsigned int MeshIndex, const aiMesh* paiMesh);
    virtual void PopulateBuffersDSA();

    struct BasicMeshEntry {
        BasicMeshEntry()
        {
            NumIndices = 0;
            BaseVertex = 0;
            BaseIndex = 0;
            MaterialIndex = 0xFFFFFFFF;
        }

        unsigned int NumIndices;
        unsigned int BaseVertex;
        unsigned int BaseIndex;
        unsigned int MaterialIndex;
    };

    std::vector<BasicMeshEntry> m_Meshes;

    const aiScene* m_pScene;

    Matrix4f m_GlobalInverseTransform;

    vector<unsigned int> m_Indices;

    enum BUFFER_TYPE {
        INDEX_BUFFER = 0,
        VERTEX_BUFFER = 1,
        WVP_MAT_BUFFER = 2,  // required only for instancing
        WORLD_MAT_BUFFER = 3,  // required only for instancing
        NUM_BUFFERS = 4
    };

    GLuint m_VAO = 0;

    GLuint m_Buffers[NUM_BUFFERS] = {0};

private:
    struct Vertex {
        Vector3f Position;
        Vector2f TexCoords;
        Vector3f Normal;
    };

    bool InitFromScene(const aiScene* pScene, const std::string& Filename);
    void CountVerticesAndIndices(const aiScene* pScene, unsigned int& NumVertices, unsigned int& NumIndices);
    void InitAllMeshes(const aiScene* pScene);
    bool InitMaterials(const aiScene* pScene, const std::string& Filename);
    void LoadTextures(const string& Dir, const aiMaterial* pMaterial, int index);

    void LoadDiffuseTexture(const string& Dir, const aiMaterial* pMaterial, int index);
    void LoadDiffuseTextureFromFile(const string& dir, const aiString& Path, int MaterialIndex);
    void LoadColors(const aiMaterial* pMaterial, int index);

    std::vector<Material> m_Materials;
    
    // Temporary space for vertex stuff before we load them into the GPU
    vector<Vertex> m_Vertices;

    Assimp::Importer m_Importer;
};


#endif
