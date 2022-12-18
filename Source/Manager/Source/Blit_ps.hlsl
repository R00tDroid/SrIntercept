#include "Convert.hlsli"

float4 main(VSOut input) : SV_Target
{
    return inputTexture.Sample(inputSampler, input.uv);
}
