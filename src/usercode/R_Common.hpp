#pragma once
#include <cstdint>
#include <unordered_map>
#include <vector>
#include "mtl_renderer.h"

enum R_BlendMode : uint32_t {
    kBlendModeDisabled,
    kBlendModeNormal = 0,
    kBlendModeAdditive,
    kBlendModeTranslucent
};

struct CMDPassData {
// depthstencil, rasterizer, etc
};

struct CMDPipelineData {
// vertex, index, uniform
};

struct CMDInstanceData {
// instance
};

class REND_CommandList {
    
};

class REND_GLSLParser {
public:
    void init();
    void addShader();
    void destroy();
private:
    
};

struct ShaderStateBuffer {
    struct ShaderStateInfo {
        uint64_t size;
        uint64_t offset;
    };
    std::vector<uint8_t> data;
    std::unordered_map<std::string, ShaderStateInfo> reflection;
    MTLBuffer gpuBuffer;
    
    void init() {
        
    }
    
    void set(const std::string& name, const uint8_t *pdata) {
        auto &info = reflection[name];
        memcpy((void*)(data.data() + info.offset), (void*)pdata, info.size);
    }
    
    void commit() {
        
    }
    
    MTLBuffer& get() {
        return gpuBuffer;
    }
};

class REND_StateManager {
public:
    
private:
    std::unordered_map<uint64_t, ShaderStateBuffer> shaderUniformBuffers;
    std::unordered_map<uint64_t, MTLShader> shaders;
};


class REND_MetalPresenter {
    
};

class REND_GLPresenter {
    
};
