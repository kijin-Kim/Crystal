#include "PBRHeader.hlsli"

cbuffer Constants : register(b0)
{
    float4x4 World : packoffset(c0);
    float4x4 ViewProj : packoffset(c4);
    float4 WorldLightPosition : packoffset(c8);
    float4 WorldCameraPosition : packoffset(c9);
};



VS_OUTPUT vsMain(VS_INPUT input)
{
    VS_OUTPUT output;

    output.Position = mul(mul(float4(input.Position,1.0f), World), ViewProj);
    output.WorldPosition = mul(float4(input.Position, 1.0f), World);

    output.WorldLightPosition = WorldLightPosition;
    output.WorldCameraPosition = WorldCameraPosition;
    output.WorldNormal = mul(input.Normal, (float3x3)World);
    output.TexCoord = input.TexCoord;
    return output;
}