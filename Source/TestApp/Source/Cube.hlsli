struct VSIn
{
    float3 Position : SV_Position;
    float3 Color : COLOR0;
};

struct VSOut
{
    float4 Position : SV_Position;
    float3 Color : COLOR0;
};

cbuffer ModelData : register(b0)
{
    float4x4 Transformation;
    float4x4 Projection;
}
