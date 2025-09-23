//
//  tahoe_management.cpp
//  wallpaper64
//
//  Created by apple on 2025-09-20.
//  Copyright © 2025 Apple. All rights reserved.
//
#include "netgame4mt/RenderEngine.hpp"
#include "netgame4mt/NetGameWorkspace.hpp"


struct glass_t {
    simd::float2 pos;
    simd::float1 size;
    simd::float2 realsize;
    simd::float1 blur;
    simd::float1 realblur;
    simd::float4 col;
};

struct glass_buffer_t {
    simd::int1 glassCount;
    simd::float2 rtSize;
    simd::float1 aspectRatio;
    glass_t glasses[512];
} glasses;


bool tahoe_button_factory(const char* text, ImVec2 size, int glass_index = 0, ImVec2 modifier = {1,1} ) {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f,0.f,0.f,0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f,0.f,0.f,0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f,0.f,0.f,0.f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.f,0.f,0.f,1.f));
    
    bool ret = ImGui::Button(text, size) ;
    
    float tsize = 0.015f, tscale = -3.f;
    
    tscale = glasses.glasses[glass_index].blur;
    tsize = glasses.glasses[glass_index].size;
        
    if (ImGui::IsItemActive()) {
        tsize = 0.02f;
      //  tscale = 2.f;
    }
    else if (ImGui::IsItemHovered()) {
        tsize = 0.03f;
       // tscale = -1.f;
    }
    else {
        tsize = 0.015f;
        tscale = -50.f;
    }
    
    auto pos = ((ImGui::GetItemRectMax() + ImGui::GetItemRectMin() ) / 2.f);
    float dx =  pos.x / (float)ImGui::GetIO().DisplaySize.x, dy = -pos.y / (float)ImGui::GetIO().DisplaySize.y ;
    dx = (dx*2.f-1.f); dy = dy*2.f+1.f;
    float speed = 10.0f; // higher = faster
    glasses.glasses[glass_index].size += (tsize - glasses.glasses[glass_index].size) * speed * ImGui::GetIO().DeltaTime;
   // glasses.glasses[glass_index].blur += (tscale - glasses.glasses[glass_index].blur) * speed * ImGui::GetIO().DeltaTime;

    float sx = size.x / (float)ImGui::GetIO().DisplaySize.x, sy = size.y / (float)ImGui::GetIO().DisplaySize.y;
    dx -= sx*modifier.x*0.5f * 1.6f; dy -= sy*modifier.y*0.5f;
   // glasses.glasses[glass_index].realsize = {sx * modifier.x * 1.6f, sy * modifier.y};
    glasses.glasses[glass_index].pos = {dx, dy};
    
    ImGui::PopStyleColor(4);

    return ret;
}


void tahoe_gui() {
    ImGui::Begin("Tahoe");
    ImGui::DragInt("count", &glasses.glassCount, 1, 0, 256);
    ImGui::InputInt("count_", &glasses.glassCount, 1, 0, 256);
    ImGui::Separator();
    for (int i = 0; i < glasses.glassCount; i++) {
        auto& glass = glasses.glasses[i];
        ImGui::PushID(i);
        ImGui::DragFloat2("pos", (float*)&glass.pos, 0.01f);
        ImGui::DragFloat("scale", (float*)&glass.blur, 0.01f);
        ImGui::DragFloat("blur", (float*)&glass.realblur, 0.01f);
        ImGui::DragFloat("size", (float*)&glass.size, 0.01f);
        ImGui::DragFloat2("xy", (float*)&glass.realsize, 0.01f);
        ImGui::ColorEdit4("color", (float*)&glass.col);
        ImGui::Separator();
        ImGui::PopID();
    }
    ImGui::End();
    
}

MTL::Buffer* pFragmentGlassBuffer ;
void tahoe_create(MTL::Device *_pDevice) {
    pFragmentGlassBuffer = _pDevice->newBuffer( sizeof(glass_buffer_t), MTL::ResourceStorageModeShared );
    RenderEngine::singleton()->buffers["uiglass"]._pBuffer = pFragmentGlassBuffer;
    {
        glasses.glassCount = 2;
        glasses.glasses[0].size = 0.018f;
        glasses.glasses[0].blur = -50.f;
        glasses.glasses[0].realblur = 16.f;
        glasses.glasses[0].col = {1.f,1.f,1.f, 1.f};
        glasses.glasses[1] = glasses.glasses[0];
        glasses.glasses[2] = glasses.glasses[0];
        glasses.glasses[3] = glasses.glasses[0];
        glasses.glasses[4] = glasses.glasses[0];
        glasses.glasses[5] = glasses.glasses[0];

        glasses.rtSize = {1.f/metalLayerWidth(), 1.f/metalLayerHeight()};
        glasses.aspectRatio = (float)metalLayerWidth()/metalLayerHeight();
        
    }
}

void tahoe_update() {
    glasses.aspectRatio = (float)metalLayerWidth()/metalLayerHeight();
    
    memcpy( pFragmentGlassBuffer->contents(), &glasses, sizeof(glass_buffer_t) );
    //pFragmentGlassBuffer->didModifyRange( NS::Range::Make( 0, pFragmentGlassBuffer->length() ) );
}
