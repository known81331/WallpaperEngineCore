//
//  scene.hpp
//  wallpaper64
//
//  Created by apple on 2025-08-28.
//  Copyright © 2025 Apple. All rights reserved.
//

#ifndef scene_hpp
#define scene_hpp

#include <string>
#include <chrono>
#include <thread>
#include <filesystem>
#include <cstdint>
#include <string>
#include <fstream>
#include <cstdio>
#include <vector>
#include <unordered_map>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}

#include "pak.h"

#include "RenderEngine.hpp"

struct Material {
    MTLTexture* texture;
    int width, height;
    int texWidth, texHeight;
};

class Render {
public:
    virtual void update() {};
    virtual void destroy() {};
    virtual ~Render() {} 
    Material material;
    std::string name;
};

class ImageRender : public Render {
public:
    simd::float3 cropOffset;
};

class ParticleRender : public Render  {
public:
    struct ParticleInstance {
        simd::float3 pos;
        simd::float3 color;
        simd::float3 velocity;
        float size;
    };
    std::vector<ParticleInstance> insts;
    
    int maxCount, rate;
    int signX, signY;
    int distanceMinX, distanceMaxX;
    int distanceMinY, distanceMaxY;
    
    int velocityMinX, velocityMaxX;
    int velocityMinY, velocityMaxY;
    int sizeMin, sizeMax;
    simd::float3 origin;
    
    void update(const simd::float3& parentPos);
};

class VideoRender : public Render {
public:
    void init(const std::string& path);
    void update() override;
    void destroy() override;
private:
    SwsContext* converter_ctx = nullptr;
    AVFormatContext* format_ctx = nullptr;
    AVCodecContext* codec_ctx = nullptr;
    AVFrame* frame, *frame2;
    int video_stream_idx = -1;
    
    double frame_duration_sec = 0.0;
    double time_accumulator = 0.0;
    long long last_update_time_ms = 0;
};

struct Model {
    uint64_t id;
    uint64_t parentId;
    Model* parent;
    simd::float3 size;
    simd::float3 scale;
    simd::float3 origin;
    int xOffset, yOffset;
    int zOrder;
    bool inheritParent;
    
    Render* render;
    
    ImVec2 recursedOrigin, recursedScale;
    
    std::string name;
};


class Scene {
public:
    struct Desc {
        int type = 0;
        std::string file;
        std::string folderPath;
        
        std::string title;
        uint64_t id;
    };
    
    void init(const Desc& desc);
    void initForPkg(const std::string& path);
    void initForVideo(const std::string& path);
    void initForWeb(const std::string& path);
    void update();
    void destroy();

    void parseMaterial(Material&, const std::string& path);
    void parseImage(ImageRender* model, const std::string& path);
    void parseParticle(ParticleRender* model, const std::string& path);
    void parseEffects(Model* model, const std::string& path);

    std::unordered_map<uint64_t, MTLTexture*> textures;
    std::unordered_map<uint64_t, Model*> modelsById;
    std::vector<Model*> models;

    std::string sceneRootPath;

};


#endif /* scene_hpp */
