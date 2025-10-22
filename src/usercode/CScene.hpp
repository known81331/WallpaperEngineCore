#pragma once

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

#include "PAK.h"
#include "Material.hpp"

class Layer {
public:
    virtual void update() {};
    virtual void destroy() {};
    virtual void draw() {};
    virtual ~Layer() {}
    Material material;
    std::string name;
};

class ImageLayer : public Layer {
public:
    simd::float3 cropOffset;
};

class ParticleLayer : public Layer  {
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
    
    Layer* layer;
    
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
    void initForPicture(const std::string& path);
    void initForWeb(const std::string& path);
    void update();
    void destroy();

    void parseMaterial(Material&, const std::string& path);
    void parseImage(ImageLayer* model, const std::string& path);
    void parseParticle(ParticleLayer* model, const std::string& path);
    void parseEffects(Model* model, const std::string& path);

    std::unordered_map<uint64_t, MTLTexture*> textures;
    std::unordered_map<uint64_t, Material*> materials;
    
    
    std::unordered_map<uint64_t, Model*> modelsById;
    std::vector<Model*> models;

    std::string sceneRootPath;

};
