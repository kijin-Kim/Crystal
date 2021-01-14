struct VS_INPUT
{
    float2 Position : POSITION;    
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float3 TexCoord : TEXCOORD;
};

cbuffer Constants : register(b0)
{
    float4x4 InverseViewProj : packoffset(c0);
};


PS_INPUT vsMain(VS_INPUT input)
{
    PS_INPUT output;
    output.Position = float4(input.Position, 1.0f, 1.0f);
    output.TexCoord = mul(output.Position, InverseViewProj).xyz;
    return output;
}


TextureCube CubemapTexture : register(t0);
SamplerState DefaultSampler : register(s0);

float4 psMain(PS_INPUT input) : SV_TARGET
{
    return CubemapTexture.Sample(DefaultSampler, input.TexCoord);
}