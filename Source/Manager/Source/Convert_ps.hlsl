#include "Convert.hlsli"

float4 main(VSOut input) : SV_Target
{
    if (conversion > 0.5f && conversion < 1.5f) // 2D
    {
        float2 uv = float2(input.uv.x / 2, input.uv.y);
        return inputTexture.Sample(inputSampler, uv);
    }
    else if (conversion > 1.5f && conversion < 2.5f) // Anaglyph
    {
        float2 leftUv = float2(input.uv.x / 2, input.uv.y);
        float2 rightUv = float2(input.uv.x / 2 +  0.5f, input.uv.y);
        float4 left = inputTexture.Sample(inputSampler, leftUv);
        float4 right = inputTexture.Sample(inputSampler, rightUv);
        return float4(left.r, right.g, right.b, 1.0f);
    }
    else if (conversion > 2.5f && conversion < 3.5f) // SBS
    {
        return inputTexture.Sample(inputSampler, input.uv);
    }

    return float4(1, 0, 0, 1);
}
