//
//  TextureLoader.cpp
//  07-textured
//
//  Created by apple on 2025-08-19.
//  Copyright © 2025 Apple. All rights reserved.
//

#include "RenderEngine.hpp"
#include "TextureLoader.hpp"
#include <FreeImage.h>

std::string GetBundleFilePath(const std::string& filename) ;

std::unordered_map<std::string, FIBITMAP*> fiTextureMap;
std::unordered_map<std::string, MTLTexture> mtlTextureMap;

void TextureLoader::init() {
    FreeImage_Initialise();
}
MTLTexture TextureLoader::createUncompressedTexture(const std::string &filePath, uint64_t fmt) {
    
    if(mtlTextureMap.find(filePath) != mtlTextureMap.end()) {
        return mtlTextureMap[filePath];
    }
    
    MTLTexture texture = {};
    
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    FIBITMAP* image = 0;

    unsigned int width(0), height(0), bpp(0);

    auto rpath = GetBundleFilePath(filePath);

        fif = FreeImage_GetFileType(filePath.c_str(), 0);
        //if still unknown, try to guess the file format from the file extension
        if (fif == FIF_UNKNOWN)
            fif = FreeImage_GetFIFFromFilename(filePath.c_str());
        //if still unkown, return failure
        if (fif == FIF_UNKNOWN) {
         //   CONSOLE.PrintText(logManager::kLogError, "Invalid texture format: %s", filePath.c_str());
            fif = FIF_TARGA;
            return {};
        }

        //check that the plugin has reading capabilities and load the file
        if (FreeImage_FIFSupportsReading(fif))
            image = FreeImage_Load(fif, rpath.c_str());
      //  else
       //     CONSOLE.PrintText(logManager::kLogError, "Unsupported texture type: %s", filePath.c_str());
        //if the image failed to load, return failure
        if (!image) {
        //    CONSOLE.PrintText(logManager::kLogError, "Failed to load texture: %s", filePath.c_str());
            //FreeImage_Unload(*image);
            return {};
        }
    
     //   image = FreeImage_Rescale(image, 256, 256);

      //  if(isFlipped)
            FreeImage_FlipVertical(image);

        //if (filePath == "rendercommon/lighting/fog_colormap.tga") {
        //    //FreeImage_Invert(*image);
        //    FreeImage_FlipVertical(*image);
        //}

        bpp = FreeImage_GetBPP(image);
        width = FreeImage_GetWidth(image);
        height = FreeImage_GetHeight(image);
        //if this somehow one of these failed (they shouldn't), return failure
        if ((bpp == 0) || (width == 0) || (height == 0)) {
            FreeImage_Unload(image);
            return {};
        }

    
    uint64_t format = 0;


        switch (bpp)
        {
        case 8:
        case 16:
        case 24:
        {
            bpp = 32;
            FIBITMAP* newImage = FreeImage_ConvertTo32Bits(image);
            FreeImage_Unload(image);
            image = newImage;
        }
        case 32:
            format = MTL::PixelFormatBGRA8Unorm;
            if (fif == FIF_DDS)
                format = MTL::PixelFormatBGRA8Unorm;
            break;
        case 128:
            {
                format = MTL::PixelFormatRGBA32Float;
            }
            break;
        default:
        {
            //we could try to handle some weird bitcount, but these will probably be HDR or some antique format, just exit instead..
            format = MTL::PixelFormatRGBA8Unorm;
      //      CONSOLE.PrintText(logManager::kLogWarning, "Unsupported bits per pixel (%ud) for texture: %s", bpp, filePath.c_str());
            //    return mTexture;
        }
        break;
        }

    fiTextureMap[filePath] = image;
    
    
    texture.create(width, height, format, MTL::TextureType2D, MTL::StorageModeManaged, MTL::TextureUsageShaderRead);
    texture.upload(bpp/8, FreeImage_GetBits(image));
    
    mtlTextureMap[filePath] = texture;

    return texture;
}

void TextureLoader::destroy() {
    FreeImage_DeInitialise();
}
