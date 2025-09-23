//
//  TextureLoader.hpp
//  07-textured
//
//  Created by apple on 2025-08-19.
//  Copyright © 2025 Apple. All rights reserved.
//

#ifndef TextureLoader_hpp
#define TextureLoader_hpp

#include <cstdint>
#include <string>

class MTLTexture;
struct FIBITMAP;

class TextureLoader {
public:
    static void init();
    static MTLTexture createUncompressedTexture(const std::string& filePath, uint64_t fmt = 0);
    static void destroy();
};

#endif /* TextureLoader_hpp */
