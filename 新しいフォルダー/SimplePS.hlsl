#include "Simple.hlsli"

float4 SimplePS(VS_Output input) : SV_TARGET
{
    float4 p = input.pos;
    //float4 color;
    //color.rg = 0.5f * (p.xy + 1.0f);
    //color.b = 1.0f;
    //color.a = 1.0f;
	
    return float4(0.5f * (p.yx + 1.0f), 1.0f, 1.0f);
}