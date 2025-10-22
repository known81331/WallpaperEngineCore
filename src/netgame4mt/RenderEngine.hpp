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
    
    std::unordered_map<std::string, MTLShader> shaders;
    std::unordered_map<std::string, MTLTexture> textures;
    std::unordered_map<std::string, MTLBuffer> buffers;
    
    std::vector<MTLDepthStencilState> depthStencilStates;
    
    
};




#endif /* renderengine_hpp */
