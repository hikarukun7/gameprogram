#include "Simple.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState      smp : register(s0);

float4 SimplePS(VS_Output input) : SV_TARGET
{
    //float3 color = tex.Sample(smp, input.uv).rgb;
    //return float4(color - fmod(color, 0.25f), 1.0f);
    return tex.Sample(smp, input.uv);
}