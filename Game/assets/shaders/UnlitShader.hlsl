

cbuffer PerFrameData : register(b0)
{
    float4x4 ViewProjection : packoffset(c0);
}


struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD;

    float4 MatRow0 : MATROW0;
    float4 MatRow1 : MATROW1;
    float4 MatRow2 : MATROW2;
    float4 MatRow3 : MATROW3;

    float3 Color : COLOR;
};

struct PS_INPUT
{
    float4 Position : SV_Position;
    nointerpolation float3 Color : COLOR;
};

PS_INPUT vsMain(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;

    float4x4 World = float4x4(input.MatRow0, input.MatRow1, input.MatRow2 ,input.MatRow3);
    output.Position = mul(mul(float4(input.Position, 1.0f), World), ViewProjection);

    output.Color = input.Color;

    return output;
}

float4 psMain(PS_INPUT input) : SV_Target
{
    return float4(input.Color, 1.0f);
}

