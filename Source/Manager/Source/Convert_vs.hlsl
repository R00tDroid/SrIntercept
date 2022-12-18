#include "Convert.hlsli"

VSOut main(VSIn input)
{
    VSOut Out;
    Out.position = float4(input.position, 0, 1);
    Out.uv = input.uv;
    return Out;
}
