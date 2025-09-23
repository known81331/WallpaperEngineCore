//
//  mtl_pipeline.cpp
//  07-textured
//
//  Created by apple on 2025-08-17.
//  Copyright © 2025 Apple. All rights reserved.
//

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define IMGUI_IMPL_METAL_CPP
#include "../gui/imgui_impl_metal.h"
#include "RenderEngine.hpp"

MTLRenderer* MTLRenderer::_pSingleton = nullptr;

void MTLRenderer::init(MTL::Device *device) {
    _pDevice = device;
    _pCommandQueue = _pDevice->newCommandQueue();
    _semaphore = dispatch_semaphore_create( 1 );
    ImGui_ImplMetal_Init(_pDevice);
}

bool tahoe_button_factory(const char* text, ImVec2 size, int glass_index = 0, ImVec2 modifier = {1,1} ) ;


void MTLRenderer::renderSequoiaPipeline(MTL::CommandBuffer* pCmd, CA::MetalDrawable* pDrawable) {
   
}
void MTLRenderer::renderGodraysPipeline(MTL::CommandBuffer* pCmd) {

}



void MTLRenderer::renderLightingPipeline(MTL::CommandBuffer* pCmd, CA::MetalDrawable* pDrawable) {
  
}


void MTLRenderer::renderSkydome(MTL::CommandBuffer* pCmd, CA::MetalDrawable* pDrawable) {
   
}

void MTLRenderer::renderTahoePipeline(MTL::CommandBuffer* pCmd, CA::MetalDrawable* pDrawable) {
    MTL::RenderPassDescriptor* pRpd = MTL::RenderPassDescriptor::alloc()->init();
    MTL::RenderPassColorAttachmentDescriptor* cd = pRpd->colorAttachments()->object(0);
    cd->setTexture(pDrawable->texture());
    cd->setLoadAction(MTL::LoadActionLoad);
    cd->setStoreAction(MTL::StoreActionStore);
    
    MTL::RenderCommandEncoder* pEnc = pCmd->renderCommandEncoder( pRpd );
    
    pEnc->setRenderPipelineState( RenderEngine::singleton()->shaders["uiglass"]._pPSO );
    pEnc->setFragmentTexture( RenderEngine::singleton()->textures["composition"]._pTexture, 0 );
    pEnc->setFragmentBuffer( RenderEngine::singleton()->buffers["uiglass"]._pBuffer, 0, 0 );
    
    
   // ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), pCmd, pEnc);
    
    
    
    pEnc->drawPrimitives( MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(6) );
    
    
  
    pEnc->endEncoding();
    pRpd->release();
}

void MTLRenderer::renderCompositionPipeline(MTL::CommandBuffer* pCmd) {
    
    MTL::RenderPassDescriptor* pRpd = MTL::RenderPassDescriptor::alloc()->init();
    MTL::RenderPassColorAttachmentDescriptor* cd = pRpd->colorAttachments()->object(0);
    cd->setTexture(RenderEngine::singleton()->textures["composition"]._pTexture);
    cd->setLoadAction(MTL::LoadActionClear);
    cd->setStoreAction(MTL::StoreActionStore);
    
    
    MTL::RenderCommandEncoder* pEnc = pCmd->renderCommandEncoder( pRpd );
    
    
    
    ImGui_ImplMetal_NewFrame(pRpd);
    
    ImGui::NewFrame();
    
    
    void NetGameImguiWindow();
    NetGameImguiWindow();
    
    ImGui::Render();
    
    ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), pCmd, pEnc);
    
    
  
    pEnc->endEncoding();
    pRpd->release();
}


void MTLRenderer::renderCompositionShadowPipeline(MTL::CommandBuffer* pCmd) {
 
}

NS::AutoreleasePool* MTLRenderer_pAutoreleasePool;
void MTLRenderer::beginFrame() {
    MTLRenderer_pAutoreleasePool = NS::AutoreleasePool::alloc()->init();

    MTL::CommandBuffer* pCmd = _pCommandQueue->commandBuffer();
    CA::MetalDrawable* pDrawable = (CA::MetalDrawable*)currentDrawable();
    
    dispatch_semaphore_wait( _semaphore, DISPATCH_TIME_FOREVER );
    MTLRenderer* pRenderer = this;
    pCmd->addCompletedHandler( ^void( MTL::CommandBuffer* pCmd ){
        dispatch_semaphore_signal( pRenderer->_semaphore );
    });
    
    renderCompositionShadowPipeline(pCmd);
    
    renderCompositionPipeline(pCmd);
    renderSequoiaPipeline(pCmd, pDrawable);
    renderLightingPipeline(pCmd, pDrawable);
    renderSkydome(pCmd, pDrawable);
    renderGodraysPipeline(pCmd);
    renderTahoePipeline(pCmd, pDrawable);
    
    pCmd->presentDrawable( pDrawable );
    pCmd->commit();

    MTLRenderer_pAutoreleasePool->release();
}

void MTLRenderer::endFrame() {
}

void MTLRenderer::destroy() {
    
}
