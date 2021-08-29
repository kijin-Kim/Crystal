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
    float4 AdditiveColor : packoffset(c4);
    float RedLevel : packoffset(c5.x);
    float GreenLevel : packoffset(c5.y);
    float BlueLevel : packoffset(c5.z);
    float Power : packoffset(c5.w);
    float Saturation : packoffset(c6.x);
    float NearStarDesaturation : packoffset(c6.y);
    float FarStarDesaturation : packoffset(c6.z);
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

    color.r *= RedLevel;
    color.g *= GreenLevel;
    color.b *= BlueLevel;
    

    float colorfraction = Saturation * -1.0f + 1.0f;
    float3 luminaceFactor = float3(0.3f, 0.59f, 0.11f);

    color = pow(color, Power);
    color = lerp(color, dot(color, luminaceFactor), colorfraction); //desaturation

    alpha *= 5.0f;
    alpha = pow(alpha, 2.0f);


    starFarColor *= 2.0f;
    starFarColor = lerp(starFarColor, dot(starFarColor, luminaceFactor), FarStarDesaturation);
    starFarColor -= alpha;
    starFarColor = saturate(starFarColor);

    starNearColor *= 2.0f;
    starNearColor = lerp(starNearColor, dot(starNearColor, luminaceFactor), NearStarDesaturation);


    float3 finalColor = color + starFarColor + starNearColor +  AdditiveColor.rgb;


    return float4(finalColor, 1.0f);
}