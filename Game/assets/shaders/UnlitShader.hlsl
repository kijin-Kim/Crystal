

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
    float2 TexCoord : TEXCOORD;

    nointerpolation float3 Color : COLOR;
};

PS_INPUT vsMain(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;

    float4x4 World = float4x4(input.MatRow0, input.MatRow1, input.MatRow2 ,input.MatRow3);
    output.Position = mul(mul(float4(input.Position, 1.0f), World), ViewProjection);

    output.Color = input.Color;

    output.TexCoord = input.TexCoord;

    return output;
}


SamplerState DefualtSampler : register(s0);
#define MAX_PER_INSTANCE_TEXTURE_COUNT 10
Texture2D Textures[MAX_PER_INSTANCE_TEXTURE_COUNT][1] : register(t0);

float4 psMain(PS_INPUT input) : SV_Target
{
    float3 color = Textures[0][0].Sample(DefualtSampler, input.TexCoord).rgb;
    return float4(color, 1.0f);
}

