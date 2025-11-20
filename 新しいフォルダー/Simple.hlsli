//頂点シェーダーからの出力、ピクセルシェーダーの入力
struct VS_Output
{
    float4 spos : SV_POSITION;
    float2 uv : TEXCOORD;
};