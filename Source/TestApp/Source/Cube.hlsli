struct VSIn
{
    float3 Position : SV_Position;
    float3 Color : TEXCOORD0;
};

struct VSOut
{
    float4 Position : SV_Position;
    float3 Color : TEXCOORD0;
};
