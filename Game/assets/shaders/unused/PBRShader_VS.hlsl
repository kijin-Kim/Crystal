#include "PBRHeader.hlsli"

cbuffer PerFrameData : register(b0)
{
    float4x4 ViewProjection;
    float4 WorldCameraPosition;
    float4 WorldLightPosition;
}

cbuffer PerObjectData : register(b1)
{
    float4x4 World;
}


VS_OUTPUT vsMain(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = mul(mul(float4(input.Position, 1.0f), World), ViewProjection);
    output.WorldPosition = mul(float4(input.Position, 1.0f), World);

    output.WorldLightPosition = WorldLightPosition;
    output.WorldCameraPosition = WorldCameraPosition;
    output.WorldNormal = mul(input.Normal, (float3x3)World);
    output.TexCoord = input.TexCoord;
    return output;
}