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

TextureCube CubemapColorTexture : register(t0);
SamplerState DefaultSampler : register(s0);

float4 psMain(PS_INPUT input) : SV_TARGET
{
    float3 color = CubemapColorTexture.Sample(DefaultSampler, input.TexCoord).rgb;


    return float4(color, 1.0f);
}