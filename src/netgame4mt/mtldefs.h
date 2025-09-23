//
//  mtldefs.h
//  07-textured
//
//  Created by apple on 2025-08-25.
//  Copyright © 2025 Apple. All rights reserved.
//

#ifndef mtldefs_h
#define mtldefs_h

#include <simd/simd.h>


typedef simd::float2 float2;
typedef simd::float3 float3;
typedef simd::float4 float4;
typedef simd::float4x4 float4x4;
typedef simd::uint1 uint;

// CONSTBUFFER DEFINITIONS FOR OLD SHADERS
struct CB_RENDERING_PERFRAME {
    float4    vDirectionalLightDirectionWorld;
    float4    vDirectionalLightDirectionView;
    float4    vTime; //X: time
    float4    vWind; //XY: wind vector
    float4    vCustomFogColorDistance; //XYZ: color, W: distance
    float4     vGodRaysColorCameraLightIntensity; //XYZ: pre multiplied god rays color W: camera light intensity
    float4     vDirectionalLightColor; //XYZ: directional light color W: directional light intensity
    float4     vPreviewLightColor; //XYZ: preview color W: empty
    float4     vParticleLightColor; //XYZ: particle color W: empty
    float4     vMainCameraWorldPosition; //XYZ: main camera world position W: empty

};

struct CB_PERFRAME
{
    float3            cb_vDirectionalLightDirectionWorld;
    float            __padd0;
    float3            cb_vDirectionalLightDirectionView;
    float            __padd1;

    float3            cb_vDirectionalLightColor;
    float            cb_vDirectionalLightIntensity;

    float            cb_fTime;
    float            cb_fTimeOfDay;
    float            cb_fGIWeight;
    float            cb_fGIBlur;

    float2            cb_vWind;
    float2            __padd2;

    float4            cb_vFogStartColor;
    float4            cb_vFogEndColor;

    float            cb_fFogStart;
    float            cb_fFogInvRange;
    float            cb_fFogFalloff;
    float            __padd3;

    float4            cb_vVerticalFogStartColor;
    float4            cb_vVerticalFogEndColor;

    float             cb_fVerticalFogInvFade;
    float            cb_fVerticalFogFalloff;
    float            cb_fVerticalFogStart;
    float            cb_fVerticalFogInvRange;

    float3            cb_vGodRaysColor;
    float            cb_fCameraLightIntensity;

    float3            cb_vCameraLightColor;
    float            cb_fCameraLightInvRange;

    float3            cb_vPreviewLightColor;
    float            cb_fBrightness;

    float3             cb_vParticleLightColor;
    float            __padd4;

    float3             cb_vMainCameraWorldPosition;
    float            __padd5;

    float             cb_fColorMin;
    float             cb_fColorRange;
    float             cb_fColorMul;
    float            __padd6;

    float4            cb_vSkinColors[16];
    float4            cb_vSubsurfaceColors[16];

};

struct CB_PROJECTION  {
    float4x4 viewToProjection;
    float4x4 projectionToView;
    float4x4 worldToView;
    float4x4 viewToWorld;
    float4x4 worldToViewProjection;
    float4x4 viewToReflection;
    float4 vNearFarViewCorner; // X: Camera Near, Y: Camera Far, Z: ViewCorner.x W: ViewCorner.y
    float4 vInverseCameraRange; // X: Inverse camera range, Y: Non-inverse camera range ZW: Unused
};

struct CB_CASCADES {
    float4x4 mCascades[4];
    float4     fSplits;
};

struct CS_RTSIZE
{
    float2 vRtSize;
    float2 vRtPixelSize;
    float fRtAspect;
    float3 _vPadding;
};

struct PointLight
{
    float3     vPosition;
    float     fScale;

    uint     uColor;
    float     fIntensity;
    float     fMaxIntensity;
    float    fFalloffFactor;

    float3    vPositionView;
    float    fInvRadius;
};

struct SpotLight
{
    float3     vPosition;
    float     fScale;

    float3     vDirection;
    float     fRange;

    float3     vPositionView;
    float     fCulledRange;

    float3     vDirectionView;
    float     fIntensity;

    uint     uColor;
    float     fCosCutoff;
    float     fCosCone;
    float     fMaxIntensity;

    float    fFalloffFactor;
    float    _padd0;
    float    _padd1;
    float    _padd2;
};

struct ShadowSpotLight
{
    float3         vPosition; // vs
    float         fScale; // vs

    float3         vDirection; // vs
    float         fRange; // ps / vs

    float3         vPositionView; // ps
    float         fCulledRange; // vs / ps

    float3         vDirectionView; // ps
    float         fIntensity; // ps

    float4x4     xViewProjection; //ps

    float         fCosCutoff; // ps
    uint         uColor; // ps
    float         fShadowFade; // ps
    float         fCosCone; // ps

    float         fMaxIntensity; // ps
    float         fFalloffFactor; // ps
    float         _padd0;
    float         _padd1;
};

struct AmbientLight
{
    float3     vPosition;
    float     fInvRadius;
    float3     vColor;
    float     fIntensity;
};

struct CB_LIGHTS
{
    SpotLight         vecSpotLights[128];
    SpotLight         vecAdditiveSpotLights[128];

    ShadowSpotLight vecShadowSpotLights[64];
    ShadowSpotLight vecAdditiveShadowSpotLights[64];

    PointLight         vecPointLights[128];
    PointLight         vecAdditivePointLights[128];

    AmbientLight     vecAmbientLights[64];

    uint            uAmbientLightCount;
    uint            _padd0;
    uint            _padd1;
    float            cb_fParticleAmbientLightMultiplier;
};

struct CB_SURFACE_INFO
{
    struct {
        uint uWidth;
        float fUvScale;
        uint _padd1;
        uint _padd2;
    } cb_info;
};

struct CB_LIGHT_CONE
{
    struct {
        float fInvDepthDist;
        float fInvNearDist;
        float fInvFarDist;
        float fEmission;
    } cb_lightCone;
};

struct CB_VISUALIZATION_COLOR
{
    struct {
        float4 vColor;

        float fFadeDistance;
        float fMaxSaturation;
        float fPulse;
        float fPulseDir;

        float fBehindInvDepthDistance;
        float fBehindOffset;
        float fInvBehindDistance;
        float _padd0;
    } cb_visualization;
};

struct CB_EFFECT_PARAMS
{
    float4 vMinColor;
    float4 vMaxColor;
};

struct ImpostorSlot
{
    float3 vCenter;
    float fRadius;
    float fPush;
    float3 _vPadding;
};

struct CB_IMPOSTORS
{
    ImpostorSlot vecImpostorSlots[256];
};

struct TileInfo
{
    float2     vWorldPos;
    float    fScale;
    uint    uEdgeFlags;
};


struct CB_TILE_INFO
{
    TileInfo arrTileInfo[512];
};

struct ClutterSlot
{
    float fHeight;
    float fScaleVariance;
    uint uFlags;
    float _fPadding;
};

struct CB_CLUTTER
{
    ClutterSlot vecClutterSlots[256];
};

struct CB_BONES {
    float4x4 xBones[1024];
};

struct CB_TRANSFORMS
{
    float4x4 transformArray[1024];
};
#endif /* mtldefs_h */
