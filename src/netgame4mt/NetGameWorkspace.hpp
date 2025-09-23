//
//  NetGameWorkspace.hpp
//  07-textured
//
//  Created by apple on 2025-08-18.
//  Copyright © 2025 Apple. All rights reserved.
//

#ifndef NetGameWorkspace_hpp
#define NetGameWorkspace_hpp

#include <simd/simd.h>
class Mesh;

class CNetGame_Workspace {
public:
    void create();
    void update();
    void destroy();
};

struct VertexData
{
    simd::float3 position;
    simd::float3 normal;
    simd::float2 texcoord;
};

struct InstanceData
{
    simd::float4x4 instanceTransform;
    simd::float3x3 instanceNormalTransform;
    simd::float4 instanceColor;
    simd::float1 time;
};

struct InstanceCPUData {
    float position[3];
    float scale[3];
};

extern InstanceData instanceDatas[10];


void WorkspaceBuildBlobulatorBuffer();
Mesh WorkspaceBuildBuffers();

Mesh WorkspaceLoadMesh(const std::string& filepath);

#endif /* NetGameWorkspace_hpp */
