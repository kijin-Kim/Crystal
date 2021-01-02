cbuffer Constants : register(b0)
{
    float4x4 World : packoffset(c0);
    float4x4 ViewProj : packoffset(c4);
    float4 WorldLightPosition : packoffset(c8);
    float4 WorldCameraPosition : packoffset(c9);
};

struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : POSITION;
    float4 WorldLightPosition : POSITION1;
    float4 WorldCameraPosition : POSITION2;
    float3 WorldNormal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

VS_OUTPUT vsMain(VS_INPUT input)
{
    VS_OUTPUT output;

    output.Position = mul(mul(float4(input.Position,1.0f), World), ViewProj);
    output.WorldPosition = mul(float4(input.Position, 1.0f), World);

    float3 LightDir = normalize(output.WorldPosition.xyz - WorldLightPosition.xyz);   

    output.WorldLightPosition = WorldLightPosition;
    output.WorldCameraPosition = WorldCameraPosition;
    output.WorldNormal = mul(input.Normal, (float3x3)World);
    output.TexCoord = input.TexCoord;
    return output;
}