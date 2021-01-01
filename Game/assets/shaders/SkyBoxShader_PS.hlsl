TextureCube cubemapTexture : register(t0);
SamplerState NormalSampler : register(s0);

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 TexCoord : TEXCOORD;
};

float4 psMain(VS_OUTPUT input) : SV_TARGET
{
    return cubemapTexture.Sample(NormalSampler, input.TexCoord);
}