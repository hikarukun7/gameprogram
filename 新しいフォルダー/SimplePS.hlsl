#include "Simple.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState      smp : register(s0);

float4 SimplePS(VS_Output input) : SV_TARGET
{
   //return float4(input.uv, 1.0f, 1.0f);
    return tex.Sample(smp, input.uv);
}