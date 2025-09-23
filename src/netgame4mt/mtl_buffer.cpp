//
//  mtl_buffer.cpp
//  07-textured
//
//  Created by apple on 2025-08-17.
//  Copyright © 2025 Apple. All rights reserved.
//

#include "mtl_renderer.h"

void MTLBuffer::create(uint64_t size, uint64_t storageMode) {
    auto* pDevice = MTLRenderer::singleton()->_pDevice;
    
#ifdef __arm64__
    if (storageMode == MTL::ResourceStorageModeManaged)
        storageMode = MTL::ResourceStorageModeShared;
#endif
    _pBuffer = pDevice->newBuffer( size, storageMode );
    this->size = size;
}

void MTLBuffer::upload(void* pData, uint64_t size, uint64_t offset) {
    memcpy( _pBuffer->contents(), (void*)((char*)pData + offset), size );
    
#ifndef __arm64__
    _pBuffer->didModifyRange( NS::Range::Make( 0, _pBuffer->length() ) );
#endif
    this->size = size;
}


void MTLBuffer::destroy() {
    _pBuffer->release();
}
