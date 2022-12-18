#include "Quad.hlsli"

Texture2D inputTexture : register(t0);
SamplerState inputSampler : register(s0);

cbuffer Info : register(b0)
{
    float conversion;
    float3 padding_;
}
