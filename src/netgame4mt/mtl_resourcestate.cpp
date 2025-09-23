//
//  mtl_resourcestate.cpp
//  07-textured
//
//  Created by apple on 2025-08-18.
//  Copyright © 2025 Apple. All rights reserved.
//

#include "mtl_renderer.h"

void MTLDepthStencilState::create(uint64_t depthCmpFunc, bool depthEnable) {
    auto* pDevice = MTLRenderer::singleton()->_pDevice;
    
    _pDesc = MTL::DepthStencilDescriptor::alloc()->init();
    _pDesc->setDepthCompareFunction( (MTL::CompareFunction)depthCmpFunc );
    _pDesc->setDepthWriteEnabled( depthEnable );
    MTL::StencilDescriptor* xx = MTL::StencilDescriptor::alloc()->init();
    xx->setWriteMask(0xFF);
    xx->setReadMask(0xFF);
    xx->setStencilCompareFunction(MTL::CompareFunctionAlways);
    xx->setDepthStencilPassOperation(MTL::StencilOperationReplace);
    xx->setStencilFailureOperation(MTL::StencilOperationReplace);
    _pDesc->setFrontFaceStencil(xx);
    _pDesc->setBackFaceStencil(xx);

    _pDepthStencilState = pDevice->newDepthStencilState( _pDesc );

}

void MTLDepthStencilState::destroy() {
    _pDepthStencilState->release();
    _pDesc->release();
}
