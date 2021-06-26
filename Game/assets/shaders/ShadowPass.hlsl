struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 BiTangent : BITANGENT;
    
    float4 MatRow0 : MATROW0;
    float4 MatRow1 : MATROW1;
    float4 MatRow2 : MATROW2;
    float4 MatRow3 : MATROW3;
};

cbuffer PerFrameData : register(b0)
{
    float4x4 LightViewProjection;
}

float4 vsMain(VS_INPUT input) : SV_Position
{
    float4x4 World = float4x4(input.MatRow0, input.MatRow1, input.MatRow2 ,input.MatRow3);
    return mul(mul(float4(input.Position, 1.0f), World), LightViewProjection);
}