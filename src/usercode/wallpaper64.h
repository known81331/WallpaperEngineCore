//
//  wallpaper64.h
//  wallpaper64
//
//  Created by apple on 2025-08-28.
//  Copyright © 2025 Apple. All rights reserved.
//

#ifndef wallpaper64_h
#define wallpaper64_h

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

#include "../netgame4mt/RenderEngine.hpp"

#include <FreeImage.h>


struct PAKImageHeader {
    char magic[8];
    uint8_t __padd1;
    char imageMagic[8];
    uint8_t __padd2;
    int textureFormat;
    int textureFlags;
    int textureWidth;
    int textureHeight;
    int width;
    int height;
    int __unk1;
    char containerMagic[8];
    uint8_t __padd3;
};

struct PAKImageContainerHeader001 {

};

struct PAKImageContainerHeader003 {
    int imageCount;
    FREE_IMAGE_FORMAT fiFormat;
};

struct PAKImageContainerHeader004 {
    int mp4;
};

struct PAKImageMipmapEntry {
    uint32_t width;
    uint32_t height;
    uint32_t compressed;
    uint32_t realSize;
    uint32_t compressedSize;
    uint8_t* data;
};

struct PAKImage {
    std::string name;
    FREE_IMAGE_FORMAT fiFormat;
    uint32_t internalFormat;
    
    uint32_t width, height;
    uint32_t texWidth, texHeight;

    int mipmapCount;
    PAKImageMipmapEntry* mips;
};

PAKImage PAKImage_Alloc(const std::string& imgPath);
void PAKImage_Free(PAKImage& img);
void PAKImage_GLUpload(PAKImage& bzImage, MTLTexture& texture);


int PAKFile_LoadAndDecompress(const char *filename);

void Wallpaper64SteamUI();

inline std::string Wallpaper64GetStorageDir() {
    return std::string(getenv("HOME")) + "/.wallpaperio/";
}

void Wallpaper64_Load(const std::string& path);

#endif /* wallpaper64_h */
