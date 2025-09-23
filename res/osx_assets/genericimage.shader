#include <metal_stdlib>

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

float linearize_depth(float d,float zNear,float zFar)
{
    return zNear * zFar / (zFar + d * (zNear - zFar));
}

half4 fragment fragmentMain( v2f in [[stage_in]],
    device const instance_data_t& instanceData [[buffer(0)]],
    texture2d< half, access::sample > tex [[texture(0)]],
    texture2d< float, access::sample > depth [[texture(1)]],
    texture2d< half, access::sample > ramp [[texture(2)]]
)
{
            
    constexpr sampler s( address::clamp_to_edge, filter::linear );
          
    float2 uv = in.position.xy * instanceData.rtSize;
    half3 nor = tex.sample(s, uv).rgb;

    float df = linearize_depth(depth.sample(s,uv).r, 0.015f, 1.f);
    half3 texel = ramp.sample(s, float2(( df + nor.r * 0.04f - nor.g * 0.06f) * 1.0f + 0.08f, 0.5f)).rgb + half3(0.14f);
    return half4( texel, 1.0 );
}
