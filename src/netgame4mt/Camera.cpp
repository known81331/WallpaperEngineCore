//
//  camera.cpp
//  07-textured
//
//  Created by apple on 2025-08-17.
//  Copyright © 2025 Apple. All rights reserved.
//

#include "RenderEngine.hpp"
#include "mtldefs.h"

CB_PROJECTION camera_data;
CB_CASCADES cascade_data;

void Camera::create() {
    cameraCBuffer.create(sizeof(CB_PROJECTION), MTL::ResourceStorageModeManaged);
    cameraCascadeCBuffer.create(sizeof(CB_CASCADES), MTL::ResourceStorageModeManaged);
}


inline void computeYawPitch(const simd::float3& here, const simd::float3& there,
                            float& outYaw, float& outPitch)
{
    float dx = there.x - here.x;
    float dy = there.y - here.y;
    float dz = there.z - here.z;

    outYaw   = std::atan2(dy, dx);                      // rotation around Z
    outPitch = std::atan2(dz, std::sqrt(dx*dx + dy*dy)); // rotation around X
}

void AimAtPoint(const simd::float3& cam, const simd::float3& at, float& y, float& x)
{
    simd::float3 target = at; //{ at.y, at.x, at.z };

    
    float xdis = target.x - cam.x;
    float ydis = target.z - cam.z;
    float zdis = target.y - cam.y;
    float xzdis = sqrtf(xdis * xdis + zdis * zdis);

    x = -atan2f(ydis, xzdis) + 1.57;
    y = -(atan2f(-xdis, zdis)) + 1.57;
}


void Camera::update() {
    velX = 0.f; velZ = 0.f;
    
    if (ImGui::IsKeyDown(ImGuiKey_W))
        velZ += 0.2f;
    if (ImGui::IsKeyDown(ImGuiKey_S))
        velZ -= 0.2f;
    if (ImGui::IsKeyDown(ImGuiKey_A))
        velX += 0.2f;
    if (ImGui::IsKeyDown(ImGuiKey_D))
        velX -= 0.2f;
    if (ImGui::IsKeyDown(ImGuiKey_Space))
        camZ += 0.2f;
    if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
        camZ -= 0.2f;
    
    camY += cosf(rotZ) * velZ + sinf(rotZ) * -velX;
    camX += -sinf(rotZ) * velZ + cosf(rotZ) * -velX;

    if (ImGui::IsKeyDown(ImGuiKey_UpArrow))
        rotX += 0.1f;
    if (ImGui::IsKeyDown(ImGuiKey_DownArrow))
        rotX -= 0.1f;
    if (ImGui::IsKeyDown(ImGuiKey_LeftArrow))
        rotZ += 0.1f;
    if (ImGui::IsKeyDown(ImGuiKey_RightArrow))
        rotZ -= 0.1f;
    

    
   camera_data.viewToProjection = math::makePerspective( 70.f * M_PI / 180.f, (float)metalLayerWidth()/metalLayerHeight(), 0.03f, 1000.0f ) ;
    camera_data.worldToView = math::makeXRotate(rotX) * math::makeZRotate(rotZ) * math::makeTranslate({-camX, -camY, -camZ}) ;
    camera_data.viewToWorld = math::inverse( camera_data.worldToView );
    camera_data.projectionToView = math::inverse(camera_data.viewToProjection);
    
    cameraCBuffer.upload(&camera_data, sizeof(CB_PROJECTION));
    
    
    float rx, rz;
    simd::float3 lightPos = {1 - camX, 3 - camY, -3 - camZ} ;
    AimAtPoint( lightPos, simd::float3{-camX,-camY,-camZ}, rz, rx);
    cascade_data.mCascades[0] = math::makeOrtho(-10.f, 10.f, -10.f, 10.f, 0.03f, 1000.f) * ( math::makeXRotate(rx) * math::makeZRotate(rz) * math::makeTranslate(lightPos) );
    
    cascade_data.fSplits.x = 2000.f;
    cameraCascadeCBuffer.upload(&cascade_data, sizeof(CB_CASCADES));
}

void Camera::destroy() {
    cameraCBuffer.destroy();
    cameraCascadeCBuffer.destroy();
}
