//
//  renderengine.cpp
//  07-textured
//
//  Created by apple on 2025-08-17.
//  Copyright © 2025 Apple. All rights reserved.
//

#include "RenderEngine.hpp"
#include "TextureLoader.hpp"
#include "../gui/imgui_impl_glfw.h"

RenderEngine* RenderEngine::_pSingleton = nullptr;

void RenderEngine::init() {
    TextureLoader::init();
    
    createShaders();
    createFramebuffers();
    createDepthStencils();
    
    
}

void RenderEngine::createFramebuffers() {
    MTLTexture tex;
    
    tex = {};
    tex.create(metalLayerWidth(), metalLayerHeight(), MTL::PixelFormatRGBA8Unorm, MTL::TextureType2D, MTL::StorageModePrivate, MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
    textures["composition"] = tex;
    
    
#ifdef __arm64__
    tex = {};
    tex.create(metalLayerWidth(), metalLayerHeight(), MTL::PixelFormatDepth16Unorm, MTL::TextureType2D, MTL::StorageModePrivate, MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
    textures["depth"] = tex;
    
    tex = {};
    tex.create(metalLayerWidth(), metalLayerHeight(), MTL::PixelFormatStencil8, MTL::TextureType2D, MTL::StorageModePrivate, MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
    textures["stencil"] = tex;
#else
    
    tex = {};
    tex.create(metalLayerWidth(), metalLayerHeight(), MTL::PixelFormatDepth24Unorm_Stencil8, MTL::TextureType2D, MTL::StorageModePrivate, MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
    textures["depth"] = tex;
    textures["stencil"] = tex;
#endif
    
}

void RenderEngine::createDepthStencils() {
    MTLDepthStencilState state = {};
    state.create(MTL::CompareFunction::CompareFunctionLess, true);
    depthStencilStates.push_back(state);
}


void RenderEngine::frame() {
    MTLRenderer::singleton()->beginFrame();
    
    
   
    
    MTLRenderer::singleton()->endFrame();

}

void RenderEngine::destroyFramebuffers() {
    textures["composition"].destroy();
    
    
#ifdef __arm64__
    textures["depth"].destroy();
    textures["stencil"].destroy();
#else
    textures["depth"].destroy();
#endif
    
}

void RenderEngine::destroy() {
    destroyFramebuffers();
}
