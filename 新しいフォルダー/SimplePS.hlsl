#include "Simple.hlsli"

float4 SimplePS(VS_Output input) : SV_TARGET
{
    return float4(input.uv, 1.0f, 1.0f);
}