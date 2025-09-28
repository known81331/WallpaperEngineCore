//
//  wallpaper64.cpp
//  wallpaper64
//
//  Created by apple on 2025-08-28.
//  Copyright © 2025 Apple. All rights reserved.
//

#include "wallpaper64.h"
#include "scene.hpp"
#include "../gui/imgui_internal.h"


std::string GetBundleFilePath(const std::string& filename);

ImVec2 recurseModelPosition(Model* mdl) ;
ImVec2 recurseModelScale(Model* mdl);

#include<mach/mach.h>
std::string MACH_ram_getvalue(){ //Note: this value is in KB!
    char line[512];

    struct task_basic_info t_info;
    mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;

    if (KERN_SUCCESS != task_info(mach_task_self(),
                                  TASK_BASIC_INFO, (task_info_t)&t_info,
                                  &t_info_count))
    {
        return "";
    }
    
    sprintf(line, "used memory: %lld\n", t_info.resident_size);

    return line;
}

void RamTile() {
    ImGui::SameLine(ImGui::GetWindowWidth() - 350);
    ImGui::MenuItem(MACH_ram_getvalue().data());
}

void HostTile() {
    char hostname[512];
    int result;
    result = gethostname(hostname, 512);
    ImGui::SameLine(ImGui::GetWindowWidth() - 410);
    ImGui::MenuItem(hostname);
    
}

void ClockTile() {
    static bool date = false;
    char ctm[128];
    time_t now = time(0);
    tm *ltm = localtime(&now);
    ImGui::SameLine(ImGui::GetWindowWidth() - 150);
    if (date)
        sprintf(ctm, "%d/%d/%d", ltm->tm_mon+1, ltm->tm_mday, ltm->tm_year % 100);
    else
        sprintf(ctm, "% 2d%c%02d %cM", ltm->tm_hour % 12 + !(ltm->tm_hour % 12) * 12 , ltm->tm_sec % 2 ? ':' : ' ', ltm->tm_min, 'A' + ltm->tm_hour / 12 * 15);

    if ( ImGui::MenuItem(ctm) )
        date = !date;
    
}

void BuildTile() {
    const char* text = __TIME__ " " __DATE__ " (ngbuild)";
    ImGui::SameLine(ImGui::GetWindowWidth() / 2 - ImGui::CalcTextSize(text).x / 2 );
    ImGui::TextColored( ImVec4(0.5f, 0.5f, 0.5f, 1.f), "%s", text);
}

void NetGameTaskbar() {
    static std::unordered_map<ImGuiWindow*, bool> hidemap = {};
    ImGui::BeginMainMenuBar();
    auto *g = ImGui::GetCurrentContext();
    
    if ( ImGui::BeginMenu("@") ) {
        for (int i = g->WindowsFocusOrder.size() - 1; i >= 0; i--) {
            auto* hwnd = g->WindowsFocusOrder[i];
            if (ImGui::MenuItem(hwnd->Name)) {
                hidemap[hwnd] = !hidemap[hwnd];
            }
        }
        ImGui::EndMenu();
    }
    
    if ( ImGui::MenuItem("WallpaperUI") ) {
        for (int i = g->WindowsFocusOrder.size() - 1; i >= 0; i--) {
            auto* hwnd = g->WindowsFocusOrder[i];
            if ( !(hwnd->Flags & ImGuiWindowFlags_NoTitleBar) )
            hidemap[hwnd] = (strcmp(hwnd->Name, "WallpaperUI")) ? true : false;
        }
    }
    
    if ( ImGui::MenuItem("editor") ) {
        for (int i = g->WindowsFocusOrder.size() - 1; i >= 0; i--) {
            auto* hwnd = g->WindowsFocusOrder[i];
            if ( !strcmp(hwnd->Name, "New WALLPAPER64") )
                hidemap[hwnd] = false;
        }
    }
    
    if ( ImGui::MenuItem("apphide") ) {
        
    }
    
    RamTile();
    HostTile();
    ClockTile();
    BuildTile();
    
    ImGui::EndMainMenuBar();
    
    for (auto& hwnd : hidemap)
        hwnd.first->Hidden = hwnd.second;
}


Scene scene;

void NetGameImguiWindow() {
    
    NetGameTaskbar();
    
    static int count = 70;
    static float _size = 1;
    static float _scale = 1.f;
    static ImVec2 camPos = {0,1200};
    static bool init = false;
    
    if (!init) {
        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text]                   = ImVec4(1.00f, 0.99f, 0.94f, 1.00f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.06f, 0.06f, 0.10f, 1.00f);
        colors[ImGuiCol_FrameBg]                = ImVec4(0.18f, 0.17f, 0.15f, 0.54f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.85f, 0.84f, 0.75f, 0.40f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(1.00f, 1.00f, 0.88f, 1.00f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_CheckMark]              = ImVec4(0.98f, 1.00f, 0.87f, 1.00f);
        colors[ImGuiCol_SliderGrab]             = ImVec4(0.99f, 0.98f, 0.90f, 1.00f);
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.15f, 0.15f, 0.12f, 1.00f);
        colors[ImGuiCol_Button]                 = ImVec4(1.00f, 0.97f, 0.86f, 1.00f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.33f, 0.33f, 0.33f, 0.45f);
        colors[ImGuiCol_Header]                 = ImVec4(0.25f, 0.24f, 0.21f, 0.31f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.20f, 0.18f, 0.15f, 0.80f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.96f, 0.96f, 0.83f, 1.00f);
        colors[ImGuiCol_TabHovered]             = ImVec4(1.00f, 0.90f, 0.71f, 0.80f);
        colors[ImGuiCol_Tab]                    = ImVec4(0.20f, 0.20f, 0.18f, 0.60f);
        colors[ImGuiCol_TabSelected]            = ImVec4(1.00f, 0.98f, 0.85f, 1.00f);
        colors[ImGuiCol_TabSelectedOverline]    = ImVec4(0.88f, 0.82f, 0.70f, 1.00f);
        colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.47f, 0.45f, 0.37f, 0.79f);
        colors[ImGuiCol_SeparatorActive]        = ImVec4(0.69f, 0.67f, 0.63f, 1.00f);
        colors[ImGuiCol_ResizeGrip]             = ImVec4(0.07f, 0.08f, 0.09f, 0.20f);
        colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.17f, 0.17f, 0.15f, 0.66f);
        colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.98f, 0.98f, 0.89f, 0.95f);
        colors[ImGuiCol_FrameBg]                = ImVec4(0.01f, 0.01f, 0.01f, 0.54f);
        colors[ImGuiCol_Button]                 = ImVec4(0.12f, 0.13f, 0.17f, 1.00f);
        colors[ImGuiCol_TabSelected]            = ImVec4(0.58f, 0.56f, 0.45f, 1.00f);




        ImGuiStyle& style = ImGui::GetStyle();
        
        style.WindowPadding.x = 8;
        style.WindowPadding.y = 8;
        style.FramePadding.x = 12;
        style.FramePadding.y = 6;
        style.ItemSpacing.x = 8;
        style.WindowTitleAlign.x = 0.05f;
        style.ChildRounding = 25;
        style.FrameRounding = 25;
        style.ScrollbarRounding = 25;
        style.GrabRounding = 25;
    
        init = true;
    }
    
    void tahoe_gui();
    tahoe_gui();
    
    ImGui::Begin("New WALLPAPER64");
    ImGui::DragInt("count", &count);
    ImGui::DragFloat("size", &_size);
    ImGui::DragFloat("scale", &_scale);
    
    ImGui::DragFloat2("cam", &camPos.x);
    
    
    for (auto* i : scene.models) {
        ImGui::PushID(i);
        ImGui::Text("%s", i->name.data());
        float cxx[2];
        cxx[0] = i->origin.x; cxx[1] = i->origin.y;
        ImGui::DragFloat2("pos", cxx);
        i->origin = {cxx[0], cxx[1]};
        
        cxx[0] = i->size.x; cxx[1] = i->size.y;
        ImGui::DragFloat2("sz", cxx);
        i->size = {cxx[0], cxx[1]};
        
        cxx[0] = i->scale.x; cxx[1] = i->scale.y;
        ImGui::DragFloat2("scale", cxx);
        i->scale = {cxx[0], cxx[1]};
        
        
        
        if (i->render && i->render->material.texture) {
            if (ImageRender* r = dynamic_cast<ImageRender*>(i->render)) {
                cxx[0] = r->cropOffset.x; cxx[1] = r->cropOffset.y;
                ImGui::DragFloat2("crop", cxx);
                r->cropOffset = {cxx[0], cxx[1]};
            }
            if (ParticleRender* r = dynamic_cast<ParticleRender*>(i->render)) {
                ImGui::Text("emitter count %ld", r->insts.size());
            }
        }
        
        ImGui::PopID();
        
        
        i->recursedScale = recurseModelScale(i);
        i->recursedOrigin = recurseModelPosition(i);
    }
    
    
    ImGui::End();
    
    ImGui::ShowDemoWindow();
   // return;
    Wallpaper64SteamUI();
    
    
    // TODO: REPLACE THIS WITH PROPER RENDERER
    
    ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
    ImGui::Begin("canva", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing);
    
    scene.update();
    {
        int c = 0;
        for (auto* i : scene.models) {
            if (c > count) break;
            if (i->render && i->render->material.texture) {
                if (ImageRender* r = dynamic_cast<ImageRender*>(i->render)) {
                    ImVec2 crop(r->cropOffset.x, r->cropOffset.y);
                    ImVec2 pos = i->recursedOrigin * _size;
                    ImVec2 size = i->recursedScale * ImVec2(i->size.x, i->size.y) * _size;
                    ImVec2 cursorPos = ImVec2(pos.x,  -pos.y) - ImVec2((size.x * 0.5f) * i->xOffset, (size.y * 0.5f)  * i->yOffset);
                    
                    ImGui::SetCursorPos(cursorPos * _scale + camPos);
                    ImGui::Image((ImTextureID)i->render->material.texture->_pTexture, size * _scale, ImVec2(0,0), ImVec2(1,1) );
                }
                else if (ParticleRender* r = dynamic_cast<ParticleRender*>(i->render)) {
                    
                    r->update({i->recursedOrigin.x, i->recursedOrigin.y, 0});
                    
                    for (auto& inst : r->insts) {
                        ImVec2 pos = ImVec2(inst.pos.x, inst.pos.y) * _size;
                        ImVec2 size = i->recursedScale * inst.size * _size;
                        ImVec2 cursorPos = ImVec2(pos.x,  -pos.y) - ImVec2((size.x * 0.5f) * i->xOffset, (size.y * 0.5f)  * i->yOffset);
                        
                        ImGui::SetCursorPos(cursorPos * _scale + camPos);
                        ImGui::Image((ImTextureID)i->render->material.texture->_pTexture, size * _scale, ImVec2(0,0), ImVec2(1,1) );
                    }
                }
                else if (VideoRender* r = dynamic_cast<VideoRender*>(i->render)) {
                        ImVec2 size = i->recursedScale * ImVec2(i->size.x, i->size.y) * _size;
                        ImGui::SetCursorPos(ImVec2(0,0));
                        ImGui::Image((ImTextureID)i->render->material.texture->_pTexture, size * _scale, ImVec2(0,0), ImVec2(1,1) );
                    }
            }
            c++;
        }
    }
    
    ImGui::PopStyleVar();
    ImGui::End();
    
    
}
