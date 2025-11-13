#include "Simple.hlsli"

VS_Output SimpleVS( float4 pos : POSITION, float2 uv : TEXCOORD ) /* : SV_POSITION */
{
    VS_Output output;
    
    output.spos = pos;
    output.uv = uv;
    
    return output;
}