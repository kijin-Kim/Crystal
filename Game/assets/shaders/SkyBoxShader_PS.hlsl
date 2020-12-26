struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
};

TextureCube cubemapTexture : register(t0);
SamplerState NormalSampler : register(s0);

cbuffer Constants : register(b0)
{
    float4x4 InverseViewProj : packoffset(c0);
};


float4 psMain(VS_OUTPUT input) : SV_TARGET
{
    float4 texCoord = mul(input.Position, InverseViewProj);
    float4 finalColor = cubemapTexture.Sample(NormalSampler, normalize(texCoord.xyz / texCoord.w));
    return finalColor;
}