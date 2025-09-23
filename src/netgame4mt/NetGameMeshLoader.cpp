//
//  NetGameMeshLoader.cpp
//  07-textured
//
//  Created by apple on 2025-08-22.
//  Copyright © 2025 Apple. All rights reserved.
//

#include "RenderEngine.hpp"
#include "NetGameWorkspace.hpp"
#include "TextureLoader.hpp"
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

std::string GetBundleFilePath(const std::string& filename) ;

InstanceData sequoiaInstanceDatas[255];
InstanceCPUData sequoiaInstanceDatasCPU[255];

Mesh WorkspaceLoadMesh(const std::string& filepath) {
    
    Mesh mesh = {};
    std::vector<VertexData> m_vertices = {};
    std::vector<uint16_t> m_indices;
    
    Assimp::Importer importer;

    unsigned int flag = //aiProcess_ConvertToLeftHanded |
            aiProcess_FindInvalidData |
            aiProcess_RemoveComponent |
            aiProcess_GenNormals |
            aiProcess_JoinIdenticalVertices |
            aiProcess_RemoveRedundantMaterials |
            aiProcess_Triangulate |
            aiProcess_CalcTangentSpace;
    
    auto rpath = GetBundleFilePath(filepath);
    
    const aiScene* scene = importer.ReadFile(rpath, flag);

    if (!scene) {
        printf(importer.GetErrorString());
        importer.FreeScene();
        return {};
    }
    
    
    
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        auto* mesh = scene->mMeshes[i];
        
        aiFace* face;


        for (unsigned int v = 0; v < mesh->mNumVertices; v++)
        {
            
            VertexData vs;
            
            vs.position = {mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z};
            if (mesh->HasNormals())
                vs.normal = {mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z};

            if (mesh->HasTextureCoords(0)) {
                vs.texcoord = { mesh->mTextureCoords[0][v].x, 1.f-mesh->mTextureCoords[0][v].y };
            }
            m_vertices.push_back(vs);
        }

        for (unsigned int f = 0; f < mesh->mNumFaces; f++)
        {
            face = &mesh->mFaces[f];
            m_indices.push_back(face->mIndices[0]);
            m_indices.push_back(face->mIndices[1]);
            m_indices.push_back(face->mIndices[2]);
        }


    }
    
    importer.FreeScene();
    
    mesh.vertexCount = m_vertices.size();
    mesh.indexCount = m_indices.size();
    
    auto vertexDataSize =  sizeof(VertexData) * mesh.vertexCount;
    auto indexDataSize = sizeof(uint16_t) * mesh.indexCount;
    
    MTLBuffer& vertexBuffer = mesh.vertexBuffer, &indexBuffer = mesh.indexBuffer;
    vertexBuffer.create(vertexDataSize, MTL::ResourceStorageModeManaged);
    indexBuffer.create(indexDataSize, MTL::ResourceStorageModeManaged);
    
    
    vertexBuffer.upload(m_vertices.data(), vertexDataSize );
    indexBuffer.upload(m_indices.data(), indexDataSize);
    
    
    mesh.instanceBuffer.create(255*sizeof(InstanceData), MTL::ResourceStorageModeManaged);
    mesh.instanceCount = 2;
    
    
    sequoiaInstanceDatas[0].instanceTransform = math::makeScale({1.f,1.f,1.f});
    sequoiaInstanceDatas[1].instanceTransform = math::makeScale({5.f,1.f,5.f}) + math::makeTranslate({8.f,2.f,0.f});
    sequoiaInstanceDatas[1].instanceColor = {1.f, 1.f, 1.f, 1.f};
    
    
    mesh.instanceBuffer.upload(sequoiaInstanceDatas, 255*sizeof(InstanceData));

    mesh.textures[0] = TextureLoader::createUncompressedTexture("obj_factory_clock_01_dif.tga");
    
    return mesh;
}
