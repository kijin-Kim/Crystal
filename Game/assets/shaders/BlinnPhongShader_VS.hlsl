cbuffer Constants : register(b0)
{
    float4x4 World : packoffset(c0);
    float4x4 ViewProj : packoffset(c4);
    float4 LightPositionInWorld : packoffset(c8);
    float4 CameraPositionInWorld : packoffset(c9);
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
    float4 PositionInWorld : POSITION;
    float4 LightPositionInWorld : POSITION1;
    float4 CameraPositionInWorld : POSITION2;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

VS_OUTPUT vsMain(VS_INPUT input)
{
    VS_OUTPUT output;

    output.Position = mul(mul(float4(input.Position,1.0f), World), ViewProj);
    output.PositionInWorld = mul(float4(input.Position, 1.0f), World);

    float3 LightDir = normalize(output.PositionInWorld.xyz - LightPositionInWorld.xyz);   

    output.LightPositionInWorld = LightPositionInWorld;
    output.CameraPositionInWorld = CameraPositionInWorld;
    output.Normal = input.Normal;
    output.TexCoord = input.TexCoord;
    return output;
}