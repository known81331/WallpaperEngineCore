//
//  renderengine.hpp
//  07-textured
//
//  Created by apple on 2025-08-17.
//  Copyright © 2025 Apple. All rights reserved.
//

#ifndef renderengine_hpp
#define renderengine_hpp

#include <cassert>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "../gui/imgui.h"

#include "mtl_renderer.h"
#include "mathlib.h"

void* currentDrawable() ;
void* renderPassDescriptor() ;
void* metalLayer() ;
int metalLayerWidth();
int metalLayerHeight();

std::string GetBundleFilePath(const std::string& filename) ;



class Camera {
public:
    void create();
    void update();
    void destroy();
    
    float camX = 0.f, camY = 0.f, camZ = 0.f;
    float rotX = 0.f, rotZ = 0.f;
    float velX = 0.f, velZ = 0.f;
    
    MTLBuffer cameraCBuffer;
    MTLBuffer cameraCascadeCBuffer;
};

struct CustomDesc {
    
};

struct MeshDesc {
    struct SubmeshDesc {
        std::string name;
        std::string textures[5];
        std::string materialName; // like DifAsgNor etc
        std::unordered_map<std::string, CustomDesc*> customs;
    };
    
    std::vector<SubmeshDesc> submeshDescriptors;
    std::string meshNames[4];

    bool useSubmeshMap = false;
    float m_maxdist = -1.f, m_mindist = -1.f, m_impostorThreshold = -1.f;

};


class Mesh {
public:
    void init(const MeshDesc&);
    MTLBuffer vertexBuffer;
    MTLBuffer indexBuffer;
    MTLBuffer instanceBuffer;
    
    uint64_t vertexCount;
    uint64_t instanceCount;
    uint64_t indexCount;
    
    MTLTexture textures[5];
    std::string materialName;
};

class RenderEngine {
public:
    static inline RenderEngine* singleton() {
        return _pSingleton;
    }
    
    static inline void initSingleton() {
        if (!_pSingleton)
            _pSingleton = new RenderEngine();
    }
    void init();
    void createShaders();
    void createFramebuffers();
    void createDepthStencils();
    void frame();
    void destroyFramebuffers();
    void destroy();
    
    static RenderEngine* _pSingleton;
    
    Camera camera;
    
    std::unordered_map<std::string, MTLShader> shaders;
    std::unordered_map<std::string, MTLTexture> textures;
    std::unordered_map<std::string, MTLBuffer> buffers;
    
    std::vector<MTLDepthStencilState> depthStencilStates;
    
    std::vector<Mesh> meshes;
    
    Mesh skydome;
    
};




#endif /* renderengine_hpp */
