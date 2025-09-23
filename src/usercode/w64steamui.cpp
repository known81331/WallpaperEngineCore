//
//  w64steamui.cpp
//  wallpaper64
//
//  Created by apple on 2025-08-30.
//  Copyright © 2025 Apple. All rights reserved.
//

#include "wallpaper64.h"
#include "scene.hpp"
#include "steam_api.h"

int _steamStatus = 0;
bool _gSteamInit = false;
bool _gSteamQueryComplete = false;

struct SteamUIQuery_t {
    EUGCQuery queryType;
    int page = 1;
} _steamUIQuery;

std::vector<SteamUGCDetails_t> details;
std::unordered_map<uint64_t, MTLTexture> details_previews;


void SteamUI_DownloadThumb(UGCHandle_t handle) ;

class CSteamUI_Handler {
public:
    void SteamUI_OnWorkshopListQueryComplete(SteamUGCQueryCompleted_t *pCallback, bool bIOFailure) {
        if (bIOFailure || pCallback->m_eResult != k_EResultOK) {
            printf("UGC query failed!\n");
            _steamStatus = 2;
            return;
        }
        
        for (int i = 0; i < 50; i++) {
            SteamUGCDetails_t detail;
            SteamUGC()->GetQueryUGCResult(pCallback->m_handle, i, &detail);
            details.push_back(detail);
            SteamUI_DownloadThumb(detail.m_hPreviewFile);
        }
        
        
        SteamUGC()->ReleaseQueryUGCRequest(pCallback->m_handle);
        _gSteamQueryComplete = true;
        
    }

    
    void SteamUI_OnThumbDownloadComplete(RemoteStorageDownloadUGCResult_t *pCallback, bool bIOFailure) {
        if (bIOFailure || pCallback->m_eResult != k_EResultOK) {
            printf("failed!\n");
            return;
        }
        puts("ugc read!!");
        char *buffer = (char*)malloc(pCallback->m_nSizeInBytes);
        int32 bytesRead = SteamRemoteStorage()->UGCRead(
            pCallback->m_hFile,
            buffer,
            pCallback->m_nSizeInBytes,
            0,
            k_EUGCRead_ContinueReadingUntilFinished
        );
        
        FIMEMORY *stream = FreeImage_OpenMemory((BYTE*)buffer, pCallback->m_nSizeInBytes);
        FIBITMAP *bmp2 = FreeImage_LoadFromMemory(FreeImage_GetFileTypeFromMemory(stream, 0), stream);
        FIBITMAP *bmp = FreeImage_ConvertTo32Bits(bmp2);
        MTLTexture tex;
        tex.create(FreeImage_GetWidth(bmp), FreeImage_GetHeight(bmp), MTL::PixelFormatBGRA8Unorm, MTL::TextureType2D, MTL::StorageModeManaged, MTL::TextureUsageShaderRead);
        tex.upload(4, FreeImage_GetBits(bmp));
        details_previews[pCallback->m_hFile] = tex;
        FreeImage_Unload(bmp);
        FreeImage_Unload(bmp2);
        FreeImage_CloseMemory(stream);
        free(buffer);
    }
    
    void SteamUI_OnItemDownloadComplete(RemoteStorageDownloadUGCResult_t *pCallback, bool bIOFailure) {
        
    }

    
    CCallResult<CSteamUI_Handler, RemoteStorageDownloadUGCResult_t> _callResult;
    CCallResult<CSteamUI_Handler, SteamUGCQueryCompleted_t> _callResult2;
};

std::vector<CSteamUI_Handler*> uiHandler_alivePool;

void SteamUI_Init() {
    _steamStatus = !SteamAPI_Init();
    
    SteamErrMsg errMsg = { 0 };
    if ( SteamAPI_InitEx( &errMsg ) != k_ESteamAPIInitResult_OK )
    {
        puts( "SteamAPI_Init() failed: " );
        puts( errMsg );
        _steamStatus = 1;
    }

    _gSteamInit = true;
}

void SteamUI_DrawCell(SteamUGCDetails_t& detail) {
    ImGui::BeginChild((uint64_t)&detail, ImVec2(256,256));
    if (details_previews[detail.m_hPreviewFile]._pTexture) {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
        ImGui::ImageButton(detail.m_rgchTitle, (ImTextureID)details_previews[detail.m_hPreviewFile]._pTexture, ImVec2(256,256), ImVec2(0, 1), ImVec2(1,0));
        ImGui::PopStyleVar();
        
        ImVec2 cursorPos = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2(128-ImGui::CalcTextSize(detail.m_rgchTitle).x/2,cursorPos.y-50));
        ImGui::Text(detail.m_rgchTitle);
    }
    ImGui::EndChild();
}


void SteamUI_DownloadThumb(UGCHandle_t handle) {
    CSteamUI_Handler* _steamUIHandler = new CSteamUI_Handler();

    SteamAPICall_t apiCall = SteamRemoteStorage()->UGCDownload(handle, 0);
    _steamUIHandler->_callResult.Set(apiCall, _steamUIHandler, &CSteamUI_Handler::SteamUI_OnThumbDownloadComplete);

    // store pointer in pool to keep alive
    uiHandler_alivePool.push_back(_steamUIHandler);
}

void SteamUI_RefreshWorkshopList(EUGCQuery ranking = k_EUGCQuery_RankedByTrend, int page = 1, bool clean = true) {
    _gSteamQueryComplete = false;
    
    if (clean) {
        details.clear();
        for (auto& i : details_previews)
            i.second.destroy();
        details_previews = {};
        
        for (auto* handler : uiHandler_alivePool) {
            delete handler;
        }
        uiHandler_alivePool.clear();
    }
    
    static UGCQueryHandle_t queryHandle;
    
    if (!_gSteamQueryComplete) {
        queryHandle = SteamUGC()->CreateQueryAllUGCRequest(ranking, k_EUGCMatchingUGCType_Items, 431960, 431960, page);
        auto apiCall = SteamUGC()->SendQueryUGCRequest(queryHandle);
        bool bFailed = false;
        
        CSteamUI_Handler* _steamUIHandler = new CSteamUI_Handler();
        
        _steamUIHandler->_callResult2.Set(apiCall, _steamUIHandler, &CSteamUI_Handler::SteamUI_OnWorkshopListQueryComplete);
        uiHandler_alivePool.push_back(_steamUIHandler);
        
    }
    
}


bool tahoe_button_factory(const char* text, ImVec2 size, int glass_index = 0, ImVec2 modifier = {0,0} ) ;

void SteamUI_WorkshopList() {
    if (_steamStatus == 1) {
        ImGui::Text("Steam is not available at this time.");
        return;
    }
    else if (_steamStatus == 2) {
        ImGui::Text("Ongoing connectivity issue detected.");
    }
    else
        ImGui::Text("Steam is online!");
   
    if (_gSteamQueryComplete) {
        
        ImGui::Begin("Navigator", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);
        ImGui::SetNextItemAllowOverlap();
        if (tahoe_button_factory("prev", ImVec2(80,80), 0))
            SteamUI_RefreshWorkshopList(k_EUGCQuery_RankedByLastUpdatedDate, --_steamUIQuery.page);
        ImGui::SameLine();
        ImGui::SetNextItemAllowOverlap();
        if (tahoe_button_factory("next", ImVec2(80,80), 1))
            SteamUI_RefreshWorkshopList(k_EUGCQuery_RankedByLastUpdatedDate, ++_steamUIQuery.page);
        ImGui::End();
        
        for (int i = 0 ; i < details_previews.size() ; i++) {
            ImGui::SetNextItemAllowOverlap();
            SteamUI_DrawCell(details[i]);
            if ( (i+1) % ((int)ImGui::GetWindowWidth() / 256) )
                ImGui::SameLine();
        }
            
    }
    
    if (details_previews.size() >= 50) {
        for (auto* handler : uiHandler_alivePool) {
            delete handler;
        }
        uiHandler_alivePool.clear();
    }
}

void SteamUI_DownloadList() {
    
}

extern Scene scene;

void Wallpaper64SteamUI() {
    if (!_gSteamInit)
        SteamUI_Init();
    
    
    SteamAPI_RunCallbacks();
    
    ImGui::Begin("WallpaperUI", 0,  ImGuiWindowFlags_NoBringToFrontOnFocus);
    
    if (ImGui::Button("unPAK")) {
        PAKFile_LoadAndDecompress("scene.pkg");
        scene.destroy();
        scene.init((Wallpaper64GetStorageDir() + "tmp_scene").data());
    }
    
    if ( ImGui::Button("Rank by trend") )
        SteamUI_RefreshWorkshopList();
    ImGui::SameLine();
    if ( ImGui::Button("Rank by date") )
        SteamUI_RefreshWorkshopList(k_EUGCQuery_RankedByLastUpdatedDate);
    
    SteamUI_WorkshopList();
    

    ImGui::End();
}

