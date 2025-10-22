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
              
                  struct glass_t {
                      float2 position;
                      float size;
                      float2 realsize;
                      float blur;
                      float realblur;
                      float4 color;
                  };
              
                  struct instance_data_t {
                      int glassCount;
                      float2 rtSize;
                      float aspectRatio;
                      glass_t glasses[512];
                  };
              
              
                  struct CameraData
                  {
                      float4x4 perspectiveTransform;
                      float4x4 worldTransform;
                      float3x3 worldNormalTransform;
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
              
                  half3 gblur( float2 uv, texture2d< half, access::sample > tex, float blur = 1.0 )
                  {
                      constexpr sampler s( address::mirrored_repeat, filter::linear );
                      float Pi = 6.28318530718; // Pi*2
                      
                      // GAUSSIAN BLUR SETTINGS {{{
                      float Directions = 16.0; // BLUR DIRECTIONS (Default 16.0 - More is better but slower)
                      float Quality = 3.0; // BLUR QUALITY (Default 4.0 - More is better but slower)
                      float Size = blur; // BLUR SIZE (Radius)
                      // GAUSSIAN BLUR SETTINGS }}}
                     
                      float2 Radius = Size/float2(2880.f,1800.f);
                      
              
                      half3 Color = tex.sample(s, uv).rgb;
                      
                      // Blur calculations
                      for( float d=0.0; d<Pi; d+=Pi/Directions)
                      {
                          for(float i=1.0/Quality; i<=1.0001; i+=1.0/Quality)
                          {
                              Color += tex.sample( s, uv+float2(cos(d),sin(d))*Radius*i).rgb;
                          }
                      }
                      
                      // Output to screen
                      Color /= Quality * Directions + 1.0;
                      return Color;
                  }
              
                      float2 rescale(float2 size, float2 texcoord) {
                          float2 mask;
                          mask.y = step(-size.y, texcoord.y) * step(texcoord.y, 0.0);
                          mask.x = step(0.0, texcoord.x) * step(texcoord.x, size.x);
                          return (1.0 - mask);
                      }

              
                       
                  half4 draw_circle(float2 texcoord_r, float2 texcoord, float2 position, float4 color, texture2d< half, access::sample > tex, float scales = 0.1f, float blur = 1.f, float radius = 1.f, float2 size = float2(1.f, 1.f), float aspx = 1.6f ) {
                      float2 sub = (texcoord-position);
                      float2 res = rescale(size, sub) * float2(aspx, 1.f);
                      sub -= float2( step(0.0, sub.x) * size.x, -step(sub.y, 0.0) * size.y );
                      float2 diff = sub * res;
                      float f = sqrt( dot( diff, diff ) / radius );
                  
                      if (f > 0.6f) return half4(0,0,0, saturate((1.0 - f - 0.2)) ) ;
                      float2 scale = float2(-scales,scales) * res;
              
                      sub = mix(0.f, sub, abs(f) );
                      
                      half3 texel1 = gblur(texcoord_r + sub* (fmod(0.56f-f, 0.6f)) , tex, blur);
                      half3 texel2 = gblur(texcoord_r + sub * ( 2.44f-(f+f) ) * (float2(14.f, 14.f) * res) * 0.098f, tex, blur);
                      half3 texel3 = gblur( texcoord_r + sub * (0.55f - f) * 0.18f * scale, tex, blur);
                      half3 texel4 = gblur(texcoord_r + sub * (0.5f - f) * 0.8f * scale, tex, blur);
                      
                      half3 texel = (f > 0.58f) ? texel2 : texel3;// * half3(color.xyz);
                      texel = min(texel, texel + texel1 * saturate((f - 0.47) / 0.23) * max(texel1, texel) );
                      texel = mix(texel3, texel, saturate((f - 0.39) / 0.35)) * max(texel4, half3(0.89f));
                      texel = mix( gblur(texcoord_r, tex, blur) * 0.89f, texel, f * 1.528f);
              
                      texel *= mix(half3(1.f), half3(color.xyz), 1.f-color.w);
                      
                      texel += half3(color.xyz) * max(min(0.7f, ( 0.26f - f) ), 0.4f) * 0.26f * color.w;
              
                          
                      return half4(texel, 1.f);
                  }

              
              
                  half4 fragment fragmentMain( v2f in [[stage_in]],
                      device const instance_data_t& instanceData [[buffer(0)]],
                      texture2d< half, access::sample > tex [[texture(0)]] )
                  {
                              
                      constexpr sampler s( address::mirrored_repeat, filter::linear );
              
                      float2 uv = in.position.xy * instanceData.rtSize;
              
                      half3 texel1 = tex.sample( s, uv ).rgb;
                      half4 circles = half4(0.f);
                      
                      for (int i = 0; i < instanceData.glassCount; i++) {
                          glass_t glass = instanceData.glasses[i];
                          half4 drawn_circle = draw_circle(uv, in.texcoord, glass.position, glass.color, tex, glass.blur, glass.realblur, glass.size, glass.realsize, instanceData.aspectRatio);
                          circles = mix(circles, drawn_circle, drawn_circle.a);
                      }
                      

                      in.color = half3( mix(texel1, circles.rgb, circles.a) ); //half3( in.color * f );
                      return half4( in.color, 1.0 );
                  }
              )", "vertexMain", "fragmentMain", {MTL::PixelFormatBGRA8Unorm});
    shaders["uiglass"] = shader;
    
}
