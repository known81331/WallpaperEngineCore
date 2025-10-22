//
//  mtl_shadercompiler.cpp
//  07-textured
//
//  Created by apple on 2025-08-17.
//  Copyright © 2025 Apple. All rights reserved.
//

#include "RenderEngine.hpp"

void RenderEngine::createShaders() {
    MTLShader shader{};
    
    shader = {};
    shader.create(R"(#include <metal_stdlib>
                  
                  using namespace metal;

                  struct v2f
                  {
                      float4 position [[position]];
                      float2 texcoord;
                      half3 color;
                  };
              
                  v2f vertex vertexMain( uint vertexId [[vertex_id]])
                  {
                      const float4 cvPositions[3] =
                      {
                          { -1.0, -1.0,  1.0,  1.0 },
                          {  3.0, -1.0,  1.0,  1.0 },
                          { -1.0,  3.0,  1.0,  1.0 },
                      };
              
                      v2f o;
                      o.position = cvPositions[ vertexId ];
                      o.texcoord = cvPositions[ vertexId ].xy;
                      o.color = half3 ( 1.f, 0.f, 0.f );
                      return o;
                  }
              
              
                  half4 fragment fragmentMain( v2f in [[stage_in]],
                      texture2d< half, access::sample > tex [[texture(0)]] )
                  {
                              
                      constexpr sampler s( address::mirrored_repeat, filter::linear );
              
                      float2 uv = in.position.xy * float2(1.f/3840.f, 1.f/2160.f); // * instanceData.rtSize;
                      return half4( tex.sample( s, uv ).rgb, 1.0 );
                  }
              )", "vertexMain", "fragmentMain", {MTL::PixelFormatBGRA8Unorm});
    shaders["uiglass"] = shader;
    
}
