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
TextureCube CubemapAlphaTexture : register(t1);
TextureCube StarFarColorTexture : register(t2);
TextureCube StarNearColorTexture : register(t3);

SamplerState DefaultSampler : register(s0);

float4 psMain(PS_INPUT input) : SV_TARGET
{
    float3 color = CubemapColorTexture.Sample(DefaultSampler, input.TexCoord).rgb;
    float3 alpha = CubemapAlphaTexture.Sample(DefaultSampler, input.TexCoord).r;
    float3 starFarColor = StarFarColorTexture.Sample(DefaultSampler, input.TexCoord).rgb;
    float3 starNearColor = StarNearColorTexture.Sample(DefaultSampler, input.TexCoord).rgb;

    

    float saturation = 1.1f;
    float colorfraction = saturation * -1.0f + 1.0f;
    float3 luminaceFactor = float3(0.3f, 0.59f, 0.11f);

    color = pow(color, 0.8f);
    color = lerp(color, dot(color, luminaceFactor), colorfraction); //desaturation

    alpha *= 5.0f;
    alpha = pow(alpha, 2.0f);


    //float starFarColorFraction = 0.3f;
    starFarColor *= 2.0f;
    //starFarColor = lerp(starFarColor, dot(starFarColor, luminaceFactor), starFarColorFraction);
    starFarColor -= alpha;
    starFarColor = saturate(starFarColor);

    starNearColor *= 2.0f;
    
    float3 additiveColor = float3(0.008364,0.012143,0.026042);

    float3 finalColor = color + starFarColor + starNearColor + additiveColor;

    return float4(finalColor, 1.0f);
}