#include "Cube.hlsli"

VSOut main(VSIn In)
{
    VSOut Out;
    Out.Position = float4(In.Position, 1);
    Out.Color = In.Color;
    return Out;
}
