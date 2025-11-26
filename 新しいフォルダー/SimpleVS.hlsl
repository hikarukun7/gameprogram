#include "Simple.hlsli"

cbuffer cbuff : register(b0)
{
    matrix mat;
}

VS_Output SimpleVS( float4 pos : POSITION, float2 uv : TEXCOORD ) /* : SV_POSITION */
{
    VS_Output output;
    
   //output.spos = pos;
    output.spos = mul(mat, pos);
    output.uv = uv;
    
    return output;
}