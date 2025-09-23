//
//  mathlib.h
//  LearnMetalCPP
//
//  Created by apple on 2025-08-17.
//  Copyright © 2025 Apple. All rights reserved.
//

#ifndef mathlib_h
#define mathlib_h

#include <simd/simd.h>


#pragma mark - Math

namespace math
{
    constexpr simd::float3 add( const simd::float3& a, const simd::float3& b )
    {
        return { a.x + b.x, a.y + b.y, a.z + b.z };
    }

    constexpr simd_float4x4 makeIdentity()
    {
        using simd::float4;
        return (simd_float4x4){ (float4){ 1.f, 0.f, 0.f, 0.f },
                                (float4){ 0.f, 1.f, 0.f, 0.f },
                                (float4){ 0.f, 0.f, 1.f, 0.f },
                                (float4){ 0.f, 0.f, 0.f, 1.f } };
    }

    inline simd::float4x4 makePerspective( float fovRadians, float aspect, float znear, float zfar )
    {
        using simd::float4;
        float ys = 1.f / tanf(fovRadians * 0.5f);
        float xs = ys / aspect;
        float zs = zfar / ( znear - zfar );
        return simd_matrix_from_rows((float4){ xs, 0.0f, 0.0f, 0.0f },
                                     (float4){ 0.0f, ys, 0.0f, 0.0f },
                                     (float4){ 0.0f, 0.0f, zs, znear * zs },
                                     (float4){ 0, 0, -1, 0 });
    }

inline simd::float4x4 lookAt(const simd::float3& eye,
                                 const simd::float3& center,
                                 const simd::float3& up)
{
    using namespace simd;

    float3  f = normalize(center - eye);
    float3  u = normalize(up);
    float3  s = normalize(cross(f, u));
    u = cross(s, f);

    return simd_matrix_from_rows(
        (float4){  s.x,  u.x, -f.x, 0.0f },
        (float4){  s.y,  u.y, -f.y, 0.0f },
        (float4){  s.z,  u.z, -f.z, 0.0f },
        (float4){ -dot(s, eye), -dot(u, eye), dot(f, eye), 1.0f }
    );
}


inline simd::float4x4 makeOrtho(float left, float right, float bottom, float top, float znear, float zfar)
{
    using simd::float4;

    float xs = 2.0f / (right - left);
    float ys = 2.0f / (top - bottom);
    float zs = 1.0f / (znear - zfar);      // match your perspective zs style

    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);
    float tz = znear / (znear - zfar);     // match your perspective tz

    return simd_matrix_from_rows(
        (float4){ xs, 0.0f, 0.0f, 0.0f },
        (float4){ 0.0f, ys, 0.0f, 0.0f },
        (float4){ 0.0f, 0.0f, zs, tz },
        (float4){ 0.0f, 0.0f, 0.0f, 1.0f }
    );
}


inline simd::float4x4 makeXRotate( float angleRadians )
    {
        using simd::float4;
        const float a = angleRadians;
        return simd_matrix_from_rows((float4){ 1.0f, 0.0f, 0.0f, 0.0f },
                                     (float4){ 0.0f, cosf( a ), sinf( a ), 0.0f },
                                     (float4){ 0.0f, -sinf( a ), cosf( a ), 0.0f },
                                     (float4){ 0.0f, 0.0f, 0.0f, 1.0f });
    }

inline simd::float4x4 makeYRotate( float angleRadians )
    {
        using simd::float4;
        const float a = angleRadians;
        return simd_matrix_from_rows((float4){ cosf( a ), 0.0f, sinf( a ), 0.0f },
                                     (float4){ 0.0f, 1.0f, 0.0f, 0.0f },
                                     (float4){ -sinf( a ), 0.0f, cosf( a ), 0.0f },
                                     (float4){ 0.0f, 0.0f, 0.0f, 1.0f });
    }

inline simd::float4x4 makeZRotate( float angleRadians )
    {
        using simd::float4;
        const float a = angleRadians;
        return simd_matrix_from_rows((float4){ cosf( a ), sinf( a ), 0.0f, 0.0f },
                                     (float4){ -sinf( a ), cosf( a ), 0.0f, 0.0f },
                                     (float4){ 0.0f, 0.0f, 1.0f, 0.0f },
                                     (float4){ 0.0f, 0.0f, 0.0f, 1.0f });
    }

inline simd::float4x4 makeTranslate( const simd::float3& v )
    {
        using simd::float4;
        const float4 col0 = { 1.0f, 0.0f, 0.0f, 0.0f };
        const float4 col1 = { 0.0f, 1.0f, 0.0f, 0.0f };
        const float4 col2 = { 0.0f, 0.0f, 1.0f, 0.0f };
        const float4 col3 = { v.x, v.y, v.z, 1.0f };
        return simd_matrix( col0, col1, col2, col3 );
    }

inline simd::float4x4 makeScale( const simd::float3& v )
    {
        using simd::float4;
        return simd_matrix((float4){ v.x, 0, 0, 0 },
                           (float4){ 0, v.y, 0, 0 },
                           (float4){ 0, 0, v.z, 0 },
                           (float4){ 0, 0, 0, 1.0 });
    }

inline simd::float3x3 discardTranslation( const simd::float4x4& m )
    {
        return simd_matrix( m.columns[0].xyz, m.columns[1].xyz, m.columns[2].xyz );
    }


inline simd::float4x4 transpose(const simd::float4x4& m)
{
    return simd_transpose(m);
}

inline simd::float4x4 inverse(const simd::float4x4& m)
{
    return simd_inverse(m);
}

inline simd::float3x3 inverse(const simd::float3x3& m)
{
    return simd_inverse(m);
}
}



#endif /* mathlib_h */
