//
//  mtl_texture.cpp
//  07-textured
//
//  Created by apple on 2025-08-17.
//  Copyright © 2025 Apple. All rights reserved.
//

#include "mtl_renderer.h"

void MTLTexture::create(uint64_t width, uint64_t height, uint64_t fmt, uint64_t type, uint64_t storageMode, uint64_t usage) {
    auto* pDevice = MTLRenderer::singleton()->_pDevice;
    
#ifdef __arm64__
    if (storageMode == MTL::StorageModeManaged)
        storageMode = MTL::StorageModeShared;
#endif
    
   _pDesc = MTL::TextureDescriptor::alloc()->init();
   _pDesc->setTextureType((MTL::TextureType)type);
   _pDesc->setStorageMode((MTL::StorageMode)storageMode );
   _pDesc->setPixelFormat((MTL::PixelFormat)fmt);
   _pDesc->setWidth(width);
   _pDesc->setHeight(height);
   _pDesc->setUsage(usage);
    if (type == MTL::TextureType2DArray)
        _pDesc->setArrayLength(4);

   _pTexture = pDevice->newTexture(_pDesc);
}

void MTLTexture::upload(uint64_t x, uint64_t y, uint64_t w, uint64_t h, uint64_t stride, uint8_t* textureData) {
    _pTexture->replaceRegion( MTL::Region( x, y, 0, w, h, 1 ), 0, textureData, w * stride );
}

void MTLTexture::upload(uint64_t stride, uint8_t* textureData) {
    _pTexture->replaceRegion( MTL::Region( 0, 0, 0, _pDesc->width(), _pDesc->height(), 1 ), 0, textureData, _pDesc->width() * stride );
}

void MTLTexture::uploadDirect(uint64_t stride, uint8_t* textureData) {
    _pTexture->replaceRegion( MTL::Region( 0, 0, 0, _pDesc->width(), _pDesc->height(), 1 ), 0, textureData, stride );
}

void MTLTexture::destroy() {
    _pTexture->release();
    _pDesc->release();
}
