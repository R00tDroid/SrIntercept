#include "Convert.hlsli"

float4 main(VSOut input) : SV_Target
{
    float4 color = inputTexture.Sample(inputSampler, input.uv);
    return float4(1, 0, 0, 1) + color;
}
