#include "Cube.hlsli"

float4 main(VSOut In) : SV_Target
{
    return float4(In.Color, 1);
}