//
//  NetGameWorkspaceMisc.cpp
//  07-textured
//
//  Created by apple on 2025-08-18.
//  Copyright © 2025 Apple. All rights reserved.
//

#include "RenderEngine.hpp"
#include "NetGameWorkspace.hpp"
#include "TextureLoader.hpp"

InstanceData instanceData;
InstanceData instanceDatas[10];

Mesh cubeMesh;
Mesh WorkspaceBuildBuffers()
{
    using simd::float2;
    using simd::float3;

    const float s = 1.f;

    VertexData verts[] = {
        //                                         Texture
        //   Positions           Normals         Coordinates
        { { -s, -s, +s }, {  0.f,  0.f,  1.f }, { 0.f, 1.f } },
        { { +s, -s, +s }, {  0.f,  0.f,  1.f }, { 1.f, 1.f } },
        { { +s, +s, +s }, {  0.f,  0.f,  1.f }, { 1.f, 0.f } },
        { { -s, +s, +s }, {  0.f,  0.f,  1.f }, { 0.f, 0.f } },

        { { +s, -s, +s }, {  1.f,  0.f,  0.f }, { 0.f, 1.f } },
        { { +s, -s, -s }, {  1.f,  0.f,  0.f }, { 1.f, 1.f } },
        { { +s, +s, -s }, {  1.f,  0.f,  0.f }, { 1.f, 0.f } },
        { { +s, +s, +s }, {  1.f,  0.f,  0.f }, { 0.f, 0.f } },

        { { +s, -s, -s }, {  0.f,  0.f, -1.f }, { 0.f, 1.f } },
        { { -s, -s, -s }, {  0.f,  0.f, -1.f }, { 1.f, 1.f } },
        { { -s, +s, -s }, {  0.f,  0.f, -1.f }, { 1.f, 0.f } },
        { { +s, +s, -s }, {  0.f,  0.f, -1.f }, { 0.f, 0.f } },

        { { -s, -s, -s }, { -1.f,  0.f,  0.f }, { 0.f, 1.f } },
        { { -s, -s, +s }, { -1.f,  0.f,  0.f }, { 1.f, 1.f } },
        { { -s, +s, +s }, { -1.f,  0.f,  0.f }, { 1.f, 0.f } },
        { { -s, +s, -s }, { -1.f,  0.f,  0.f }, { 0.f, 0.f } },

        { { -s, +s, +s }, {  0.f,  1.f,  0.f }, { 0.f, 1.f } },
        { { +s, +s, +s }, {  0.f,  1.f,  0.f }, { 1.f, 1.f } },
        { { +s, +s, -s }, {  0.f,  1.f,  0.f }, { 1.f, 0.f } },
        { { -s, +s, -s }, {  0.f,  1.f,  0.f }, { 0.f, 0.f } },

        { { -s, -s, -s }, {  0.f, -1.f,  0.f }, { 0.f, 1.f } },
        { { +s, -s, -s }, {  0.f, -1.f,  0.f }, { 1.f, 1.f } },
        { { +s, -s, +s }, {  0.f, -1.f,  0.f }, { 1.f, 0.f } },
        { { -s, -s, +s }, {  0.f, -1.f,  0.f }, { 0.f, 0.f } }
    };

    uint16_t indices[] = {
         0,  1,  2,  2,  3,  0, /* front */
         4,  5,  6,  6,  7,  4, /* right */
         8,  9, 10, 10, 11,  8, /* back */
        12, 13, 14, 14, 15, 12, /* left */
        16, 17, 18, 18, 19, 16, /* top */
        20, 21, 22, 22, 23, 20, /* bottom */
    };

    const size_t vertexDataSize = sizeof( verts );
    const size_t indexDataSize = sizeof( indices );

    Mesh mesh = {};
    MTLBuffer& vertexBuffer = mesh.vertexBuffer, &indexBuffer = mesh.indexBuffer;
    vertexBuffer.create(vertexDataSize, MTL::ResourceStorageModeManaged);
    indexBuffer.create(indexDataSize, MTL::ResourceStorageModeManaged);
    
    vertexBuffer.upload(verts, vertexDataSize);
    indexBuffer.upload(indices, indexDataSize);
    
    mesh.vertexCount = 24;
    mesh.indexCount = 36;
    
    mesh.instanceBuffer.create(10*sizeof(InstanceData), MTL::ResourceStorageModeManaged);
    mesh.instanceCount = 2;
    
    
    instanceDatas[0].instanceTransform = math::makeScale({1.f,1.f,1.f});
    instanceDatas[1].instanceTransform = math::makeScale({100.f,100.f,1.f}) + math::makeTranslate({0.f,2.f,0.f});
    instanceDatas[1].instanceColor = {1.f, 1.f, 1.f, 1.f};
    
    
    mesh.instanceBuffer.upload(instanceDatas, 10*sizeof(InstanceData));
    
    mesh.textures[0] = TextureLoader::createUncompressedTexture("gnd_grass_dif.tga");
 
    cubeMesh = mesh;
    
    return mesh;
}

void WorkspaceCubeSetPosition(int i, simd::float3 pos) {
    instanceDatas[i].instanceTransform =  math::makeScale({1.f,1.f,1.f}) * math::makeTranslate(pos);
    cubeMesh.instanceBuffer.upload(instanceDatas, 10*sizeof(InstanceData));
}

void WorkspaceBuildBlobulatorBuffer() {
    
    Mesh mesh = {};
    
    mesh.vertexBuffer.create(1000000 * sizeof(VertexData), MTL::ResourceStorageModeManaged);
    mesh.vertexCount = 3;
    mesh.instanceBuffer.create(sizeof(InstanceData), MTL::ResourceStorageModeManaged);
    mesh.instanceCount = 1;
    
    instanceData.instanceTransform = math::makeScale({1.f,1.f,1.f});
    instanceData.instanceNormalTransform = math::discardTranslation(instanceData.instanceTransform);
    instanceData.instanceColor = {1.f, 0.6f, 1.f};
    mesh.instanceBuffer.upload(&instanceData, sizeof(InstanceData));
    
    mesh.textures[0] = TextureLoader::createUncompressedTexture("7jx.png");
    
    RenderEngine::singleton()->meshes.push_back(mesh);
}
