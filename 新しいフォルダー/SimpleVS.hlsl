#include "Simple.hlsli"

VS_Output SimpleVS( float4 pos : POSITION ) /* : SV_POSITION */
{
    VS_Output output;
	
    output.spos = pos;
    output.pos = pos;
	
	return output;
}