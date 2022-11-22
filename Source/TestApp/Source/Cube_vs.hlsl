#include "Cube.hlsli"

VSOut main(VSIn In)
{
    float4x4 ModelProj = mul(Transformation, Projection);

    VSOut Out;
    Out.Position = mul(float4(In.Position, 1), ModelProj);
    Out.Color = In.Color;
    return Out;
}
