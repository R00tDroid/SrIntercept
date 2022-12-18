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
