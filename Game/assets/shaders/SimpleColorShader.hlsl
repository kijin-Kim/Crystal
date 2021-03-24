

cbuffer PerFrameData : register(b0)
{
    float4x4 ViewProjection : packoffset(c0);
}

cbuffer PerObjectData : register(b1)
{
    float4x4 World;
    float3 Color;
}

float4 vsMain(float3 Position : POSITION) : SV_POSITION
{
    return mul(mul(float4(Position, 1.0f), World), ViewProjection);
}

float4 psMain() : SV_Target
{
    return float4(Color, 1.0f);
}

