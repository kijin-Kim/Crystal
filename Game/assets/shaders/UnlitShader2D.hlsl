

cbuffer PerFrameData : register(b0)
{
    float4x4 View : packoffset(c0);
    float4x4 Projection : packoffset(c4);
}

cbuffer PerObjectData : register(b1)
{
    float4x4 World : packoffset(c0);
    float3 AlbedoColor : packoffset(c4.x);
    bool bToggleAlbedoTexture : packoffset(c4.w);
    float3 TintColor : packoffset(c5.x);
    float Opacity : packoffset(c5.w);
    float OpacityMultiplier : packoffset(c6.x);
    bool bToggleOpacityTexture : packoffset(c6.y);
    bool bUseAlbedoTextureAlpha : packoffset(c6.z);
}

struct VS_INPUT
{
    float2 Position : POSITION;
};

struct PS_INPUT
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD;
};


PS_INPUT vsMain(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;

    output.Position = mul(mul(mul(float4(input.Position, 0.0f, 1.0f), World), View), Projection);
    
    output.TexCoord = ((input.Position + 1.0f) / 2.0f);
	output.TexCoord.y = -output.TexCoord.y;
    return output;
}


Texture2D AlbedoTexture : register(t0);
Texture2D OpacityTexture  : register(t1);

SamplerState DefaultSampler : register(s0);

float4 psMain(PS_INPUT input) : SV_Target
{

    float4 albedoColor = bToggleAlbedoTexture ? AlbedoTexture.Sample(DefaultSampler, input.TexCoord) : float4(AlbedoColor, 1.0f);
    float opacity = 0.0f;
    if(!bUseAlbedoTextureAlpha)
    {
        opacity = bToggleOpacityTexture ? OpacityTexture.Sample(DefaultSampler, input.TexCoord).r : Opacity;
    }
    else
    {
        opacity = albedoColor.a;
    }

    albedoColor.rgb *= TintColor;
    opacity *= OpacityMultiplier;

    float4 finalColor = float4(albedoColor.rgb, opacity);
    return finalColor;
}

