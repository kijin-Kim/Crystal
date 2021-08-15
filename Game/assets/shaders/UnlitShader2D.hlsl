

cbuffer PerFrameData : register(b0)
{
    float4x4 View : packoffset(c0);
    float4x4 Projection : packoffset(c4);
}

struct VS_INPUT
{
    float2 Position : POSITION;
    float4 MatRow0 : MATROW0;
    float4 MatRow1 : MATROW1;
    float4 MatRow2 : MATROW2;
    float4 MatRow3 : MATROW3;


    bool bToggleAlbedoTexture : TOGGLE_ALBEDO_TEXTURE;
    bool bToggleOpacityTexture : TOGGLE_OPACITY_TEXTURE;
    bool bUseAlbedoTextureAlpha : USE_ALBEDO_ALPHA;
    float3 AlbedoColor : ALBEDO_COLOR;
    float Opacity : OPACITY;
    float OpacityMultiplier : OPACITY_MULTIPLIER;
};

struct PS_INPUT
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD;

    
    nointerpolation bool bToggleAlbedoTexture : TOGGLE_ALBEDO_TEXTURE;
    nointerpolation bool bToggleOpacityTexture : TOGGLE_OPACITY_TEXTURE;
    nointerpolation bool bUseAlbedoTextureAlpha : USE_ALBEDO_ALPHA;

    nointerpolation float3 AlbedoColor : ALBEDO_COLOR;
    nointerpolation float Opacity : OPACITY;
    nointerpolation float OpacityMultiplier : OPACITY_MULTIPLIER;
};


PS_INPUT vsMain(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;

    
    float4x4 World = float4x4(input.MatRow0, input.MatRow1, input.MatRow2, input.MatRow3);

    output.Position = mul(mul(mul(float4(input.Position, 0.0f, 1.0f), World), View), Projection);
    
    output.TexCoord = ((input.Position + 1.0f) / 2.0f);
	output.TexCoord.y = -output.TexCoord.y;

    output.bToggleAlbedoTexture = input.bToggleAlbedoTexture;
    output.bToggleOpacityTexture = input.bToggleOpacityTexture;
    output.bUseAlbedoTextureAlpha = input.bUseAlbedoTextureAlpha;
    output.AlbedoColor = input.AlbedoColor;
    output.Opacity = input.Opacity;
    output.OpacityMultiplier = input.OpacityMultiplier;

    return output;
}


Texture2D AlbedoTexture : register(t0);
Texture2D OpacityTexture  : register(t1);

SamplerState DefaultSampler : register(s0);

float4 psMain(PS_INPUT input) : SV_Target
{

    float4 albedoColor = input.bToggleAlbedoTexture ? AlbedoTexture.Sample(DefaultSampler, input.TexCoord) : float4(input.AlbedoColor, 1.0f);
    float opacity = 0.0f;
    if(!input.bUseAlbedoTextureAlpha)
    {
        opacity = input.bToggleOpacityTexture ? OpacityTexture.Sample(DefaultSampler,  input.TexCoord).r : input.Opacity;
    }
    else
    {
        opacity = albedoColor.a;
    }

    opacity *= input.OpacityMultiplier;

    float4 finalColor = float4(albedoColor.rgb, opacity);
    return finalColor;
}

