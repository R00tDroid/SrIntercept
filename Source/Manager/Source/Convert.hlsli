Texture2D inputTexture : register(t0);
SamplerState inputSampler : register(s0);

struct VSIn
{
    float2 position : SV_Position;
    float2 uv : TEXCOORD0;
};

struct VSOut
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

cbuffer Info : register(b0)
{
    float conversion;
    float3 padding_;
}
