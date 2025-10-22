//
//  mtl_shader.cpp
//  07-textured
//
//  Created by apple on 2025-08-17.
//  Copyright © 2025 Apple. All rights reserved.
//

#include "mtl_renderer.h"
#include <fstream>
#include <sstream>

std::string GetBundleFilePath(const std::string& filename) ;

void MTLShader::create(const std::string& code, const std::string& entryVS, const std::string& entryPS, const std::vector<uint64_t>& attachmentFmt, uint64_t depthFmt) {
    auto* pDevice = MTLRenderer::singleton()->_pDevice;
    
    auto NSCode = NS::String::string(code.data(), NS::UTF8StringEncoding);
    
    if (code.size() < 96) {
        std::string rpath = GetBundleFilePath(code);
        std::ifstream str(rpath.data());
        
        if (!str.is_open()) {
            assert( false );
        }
        
        std::ostringstream buffer;
        buffer << str.rdbuf();
        
        std::string contents = buffer.str(); // hold it in a variable
        NSCode = NS::String::string(contents.c_str(), NS::UTF8StringEncoding);
    }
    
    
    NS::Error* pError = nullptr;
    _pShaderLibrary = pDevice->newLibrary( NSCode, nullptr, &pError );
    
    if ( !_pShaderLibrary ) {
        __builtin_printf( "%s", pError->localizedDescription()->utf8String() );
        assert( false );
    }
    

    MTL::Function* pVertexFn = _pShaderLibrary->newFunction( NS::String::string(entryVS.data(), NS::UTF8StringEncoding) );
    MTL::Function* pFragFn = _pShaderLibrary->newFunction( NS::String::string(entryPS.data(), NS::UTF8StringEncoding) );
    
    _pDesc = MTL::RenderPipelineDescriptor::alloc()->init();
    _pDesc->setVertexFunction( pVertexFn );
    _pDesc->setFragmentFunction( pFragFn );
    if (attachmentFmt.size() > 0) {
        for (int i = 0; i < attachmentFmt.size(); i++)
            _pDesc->colorAttachments()->object(i)->setPixelFormat( (MTL::PixelFormat)attachmentFmt[i]);
    }
    else
        _pDesc->colorAttachments()->object(0)->setPixelFormat( MTL::PixelFormat::PixelFormatRGBA8Unorm );
    
    if (depthFmt != UINT64_MAX) {
        _pDesc->setDepthAttachmentPixelFormat( (MTL::PixelFormat)depthFmt );
        if ( depthFmt == MTL::PixelFormatDepth24Unorm_Stencil8) {
#ifdef __arm64__
            _pDesc->setDepthAttachmentPixelFormat( MTL::PixelFormatDepth16Unorm );
            _pDesc->setStencilAttachmentPixelFormat(MTL::PixelFormatStencil8);
#else
            _pDesc->setStencilAttachmentPixelFormat(MTL::PixelFormatDepth24Unorm_Stencil8);
#endif
        }
    }

    _pPSO = pDevice->newRenderPipelineState( _pDesc, &pError );
}

void MTLShader::destroy() {
    _pShaderLibrary->release();
    _pPSO->release();
    _pDesc->release();
}
